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

#ifndef STORAGE_DATABASE_SQLITE_H_
#define STORAGE_DATABASE_SQLITE_H_

#include "storage/storage_database.h"

struct sqlite3;
struct sqlite3_stmt;

namespace Farm {

class SQLiteStorage : public DatabaseStorage {
 public:
  explicit SQLiteStorage(string filename);

  /* Perform connection to the storage. */
  bool connect();

  /* Disconnect from the storage. */
  bool disconnect();

  /* Create or update database schema.
   * It will also prepare all statmenets.
   */
  bool create_schema();

  /* Retrieve all jobs from the storage. */
  bool retrieve_all_jobs(vector<Job*> *all_jobs);

  /* Retrieve all tasks of a given job from the storage. */
  bool retrieve_all_tasks(const Job& job,
                          vector<Task*> *all_tasks);

  /* Insert new job into the database. */
  bool insert_job(Job *job);

  /* Update job in the stroage. */
  bool update_job(const Job& job);

  /* Update task in the stroage. */
  bool update_task(const Task& task);

  /* Fliush caches to the actual storage. */
  bool flush_caches(bool force = false);

  /* ** Performance parameters ** */

  /* Combine update database requests together when they're happening
   * too often in order to increase throughput.
   */
  bool use_bulked_transactions;

  /* This parameter denotes how often transactions are being applied on the
   * actual database when using bulked transactions.
   */
  double transaction_commit_interval;

 protected:
  /* Begin new transaction. */
  void transaction_begin();

  /* Rollback current transaction. */
  void transaction_rollback();

  /* Commit current transaction. */
  void transaction_commit();

  /* Begin new transaction, does nothing if transaction is aleady started. */
  void transaction_begin_pending();

  /* Commit possibly pending transaction. */
  void transaction_commit_pending(bool force = false);

  /* Prepare statement. */
  sqlite3_stmt *sql_prepare(string sql);

  /* Execute gived SQL.
   *
   * Simply a wrapper around sqlite3_exec() which does
   * some error checking and allows to use string instead
   * of char* as an SQL which is handy.
   */
  bool sql_exec(string sql);

  /* Execute prepared statement.
   *
   * Equals to running sqlite3_step() and sqlite3_reset().
   * Does error checks of the results.
   */
  bool sql_exec_prepared(sqlite3_stmt *statement);

  /* File name of the database. */
  string filename_;
  /* Database descriptor. */
  sqlite3 *database_;

  /* Denotesi if there're any open transactions. */
  bool has_open_transaction_;

  /* Timestamp of currently opened transaction. */
  bool transaction_open_timestamp_;

  /* Prepared statements.
   *
   * Used for faster queries, so commonly used queries are only
   * having parameters rebound without need of parsing statment
   * on every execution.
   */
  sqlite3_stmt *select_all_jobs_statement_;
  sqlite3_stmt *select_job_tasks_statement_;
  sqlite3_stmt *insert_job_statement_;
  sqlite3_stmt *insert_task_statement_;
  sqlite3_stmt *update_job_statement_;
  sqlite3_stmt *update_task_statement_;
};

} /* namespace Farm */

#endif  /* STORAGE_DATABASE_H_ */
