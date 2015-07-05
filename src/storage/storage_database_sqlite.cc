// Copyright (c) 2015 farm-proto authors.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

#include "storage/storage_database_sqlite.h"

#include <cassert>

#include "sqlite/sqlite3.h"
#include "util/util_foreach.h"
#include "util/util_logging.h"
#include "util/util_time.h"

/* Cofigure sqlite to b as fast as possible, need reliable back-UPSed
 * computer to prevent database corruption in the event of major
 * failure.
 */
// #define MAXIMUM_PERFORMANCE

namespace Farm {

SQLiteStorage::SQLiteStorage(string filename)
    : filename_(filename),
      database_(NULL),
      has_open_transaction_(false),
      select_all_jobs_statement_(NULL),
      select_job_tasks_statement_(NULL),
      insert_job_statement_(NULL),
      insert_task_statement_(NULL),
      update_task_statement_(NULL) {
  VLOG(1) << "Using SQLite version " << sqlite3_libversion();
  /* Those are tweakable performance parameters.
   * By default we do maximum reliability.
   */
  use_bulked_transactions = false;
  transaction_commit_interval = 2.0;
}

bool SQLiteStorage::create_schema() {
  /* Make sure all tables exists. */
  sql_exec("CREATE TABLE IF NOT EXISTS jobs("
               "id INTEGER PRIMARY KEY ASC, "
               "priority INT, "
               "status INT, "
               "name TEXT_);");
  sql_exec("CREATE TABLE IF NOT EXISTS tasks("
               "id INTEGER PRIMARY KEY ASC, "
               "job_id INT, "
               "status INT);");

  /* Prepare statements, */
  select_all_jobs_statement_ =
        sql_prepare("SELECT id, priority, status, name FROM jobs");
  select_job_tasks_statement_ =
        sql_prepare("SELECT id, status FROM tasks WHERE job_id=?");
  insert_job_statement_ =
        sql_prepare("INSERT INTO jobs VALUES(NULL, ?, ?, ?)");
  insert_task_statement_ =
        sql_prepare("INSERT INTO tasks VALUES(NULL, ?, ?)");
  update_job_statement_ =
        sql_prepare("UPDATE jobs SET priority=?, status=?, name=? WHERE id=?");
  update_task_statement_ =
        sql_prepare("UPDATE tasks SET status=? WHERE id=?");

  return true;
}

/* Perform connection to the storage. */
bool SQLiteStorage::connect() {
  int rc = sqlite3_open(filename_.c_str(), &database_);
  if(rc != SQLITE_OK) {
    LOG(FATAL) << "Cannot open database" << sqlite3_errmsg(database_);
    sqlite3_close(database_);
    return false;
  }
  VLOG(1) << "Successfully connected to " << filename_;
  /* ** Optimization tricks. ** */
  /* This optimizations are failure-safe. */
  sql_exec("PRAGMA count_changes=OFF");
  sql_exec("PRAGMA locking_mode=EXCLUSIVE");
#ifndef MAXIMUM_PERFORMANCE
  /* This is almost failure-safe optimizations, probability of their failure
   * is quite the same as meteor hitting your render farm.
   */
  // sql_exec("PRAGMA synchronous=NORMAL");
#else
  /* This optimizations are not safe in thecase of application crash or
   * power outage, but still possible to use those for maximum performance.
   */
  sql_exec("PRAGMA journal_mode=MEMORY");
  sql_exec("PRAGMA synchronous=OFF");
#endif
  return true;
}

/* Disconnect from the storage. */
bool SQLiteStorage::disconnect() {
  VLOG(1) << "Disconnecting from " << filename_ << ".";
  transaction_commit_pending(true);
  sqlite3_finalize(select_all_jobs_statement_);
  sqlite3_finalize(select_job_tasks_statement_);
  sqlite3_finalize(insert_job_statement_);
  sqlite3_finalize(insert_task_statement_);
  sqlite3_finalize(update_job_statement_);
  sqlite3_finalize(update_task_statement_);
  sqlite3_close(database_);
  return true;
}

/* Retrieve all jobs from the storage. */
bool SQLiteStorage::retrieve_all_jobs(vector<Job*> *all_jobs) {
  int rc;
  all_jobs->clear();
  while((rc = sqlite3_step(select_all_jobs_statement_)) == SQLITE_ROW) {
    int id = sqlite3_column_int(select_all_jobs_statement_, 0);
    Job::Priority priority =
          (Job::Priority)sqlite3_column_int(select_all_jobs_statement_, 1);
    Job::Status status =
          (Job::Status)sqlite3_column_int(select_all_jobs_statement_, 2);
    const char *name =
          (const char*)sqlite3_column_text(select_all_jobs_statement_, 3);
    Job *new_job = new Job(id, priority, status, name);
    all_jobs->push_back(new_job);
  }
  sqlite3_reset(select_all_jobs_statement_);
  return true;
}

/* Retrieve all tasks of a given job from the storage. */
bool SQLiteStorage::retrieve_all_tasks(const Job& job,
                                       vector<Task*> *all_tasks) {
  int rc;
  all_tasks->clear();
  sqlite3_bind_int(select_job_tasks_statement_, 1, job.id());
  while((rc = sqlite3_step(select_job_tasks_statement_)) == SQLITE_ROW) {
    int id = sqlite3_column_int(select_job_tasks_statement_, 0);
    Task::Status status =
          (Task::Status)sqlite3_column_int(select_job_tasks_statement_, 1);
    Task *new_task = new Task(id, status);
    all_tasks->push_back(new_task);
  }
  sqlite3_reset(select_job_tasks_statement_);
  return true;
}

/* Insert new job into the database. */
bool SQLiteStorage::insert_job(Job *job) {
  VLOG(1) << "Inserting new job: " << job->name() << ".";
  /* Force apply all the pending transactions. */
  transaction_commit_pending(true);
  sqlite3_bind_int(insert_job_statement_, 1, job->priority());
  sqlite3_bind_int(insert_job_statement_, 2, job->status());
  sqlite3_bind_text(insert_job_statement_, 3,
                    job->name().c_str(),
                    job->name().size(),
                    SQLITE_TRANSIENT);
  transaction_begin();
  if(!sql_exec_prepared(insert_job_statement_)) {
    transaction_rollback();
    return false;
  }
  job->set_id(sqlite3_last_insert_rowid(database_));
  vector<Task*> &tasks = job->tasks();
  foreach(Task *task, tasks) {
    sqlite3_bind_int(insert_task_statement_, 1, job->id());
    sqlite3_bind_int(insert_task_statement_, 2, task->status());
    if(!sql_exec_prepared(insert_task_statement_)) {
      transaction_rollback();
      return false;
    }
    task->set_id(sqlite3_last_insert_rowid(database_));
  }
  transaction_commit();
  return true;
}

/* Update job in the stroage. */
bool SQLiteStorage::update_job(const Job& job) {
  sqlite3_bind_int(update_job_statement_, 1, job.priority());
  sqlite3_bind_int(update_job_statement_, 2, job.status());
  sqlite3_bind_text(update_job_statement_, 3,
                    job.name().c_str(),
                    job.name().size(),
                    SQLITE_TRANSIENT);
  sqlite3_bind_int(update_job_statement_, 4, job.id());
  return sql_exec_prepared(update_job_statement_);
}

/* Update task in the stroage. */
bool SQLiteStorage::update_task(const Task& task) {
  transaction_begin_pending();
  sqlite3_bind_int(update_task_statement_, 1, task.status());
  sqlite3_bind_int(update_task_statement_, 2, task.id());
  return sql_exec_prepared(update_task_statement_);
  transaction_commit_pending();
}

/* Fliush caches to the actual storage. */
bool SQLiteStorage::flush_caches(bool force) {
  transaction_commit_pending(force);
  return true;
}

/* Helper functions, wrappers around more low-level calls. */

/* Begin new transaction. */
void SQLiteStorage::transaction_begin() {
  assert(has_open_transaction_ == false);
  sql_exec("BEGIN TRANSACTION");
  has_open_transaction_ = true;
  transaction_open_timestamp_ = util_time_dt();
}

/* Rollback current transaction. */
void SQLiteStorage::transaction_rollback() {
  assert(has_open_transaction_ == true);
  sql_exec("ROLLBACK TRANSACTION");
  has_open_transaction_ = false;
}

/* Commit current transaction. */
void SQLiteStorage::transaction_commit() {
  assert(has_open_transaction_ == true);
  sql_exec("END TRANSACTION");
  has_open_transaction_ = false;
}

void SQLiteStorage::transaction_begin_pending() {
  if(use_bulked_transactions && !has_open_transaction_) {
    VLOG(1) << "Starting new pending transaction.";
    transaction_begin();
  }
}

/* Commit possibly pending transaction. */
void SQLiteStorage::transaction_commit_pending(bool force) {
  if(use_bulked_transactions && has_open_transaction_) {
    double time_dt = util_time_dt() - transaction_open_timestamp_;
    if(force || time_dt  >= transaction_commit_interval) {
      VLOG(1) << "Comitting pending transaction.";
      transaction_commit();
    }
  }
}

sqlite3_stmt *SQLiteStorage::sql_prepare(string sql) {
  sqlite3_stmt *statement;
  int rc = sqlite3_prepare(database_,
                           sql.c_str(),
                           sql.size(),
                           &statement,
                           NULL);
  if(rc != SQLITE_OK) {
    LOG(FATAL) << "Error preparing statement " << "'" << sql << "', "
               << sqlite3_errstr(rc) << ".";
    return NULL;
  }
  return statement;
}

bool SQLiteStorage::sql_exec(string sql) {
  VLOG(2) << "Executing query: " << sql;
  char *error_message = 0;
  int rc = sqlite3_exec(database_, sql.c_str(), 0, 0, &error_message);
  if(rc != SQLITE_OK) {
    LOG(FATAL) << "Failed to execute query: " << sql
               << ", error: " << error_message << ".";
    sqlite3_free(error_message);
    return false;
  }
  return true;
}

bool SQLiteStorage::sql_exec_prepared(sqlite3_stmt *statement) {
  int rc = sqlite3_step(statement);
  if(rc != SQLITE_DONE) {
    VLOG(1) << rc;
    LOG(FATAL) << "Failed to execute prepared query, "
               << sqlite3_errstr(rc) << ".";
    return false;
  }
  sqlite3_reset(statement);
  return true;
}

} /* namespace Farm */
