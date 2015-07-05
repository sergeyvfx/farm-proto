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

#include "storage/storage_dryrun.h"

#include "util/util_foreach.h"
#include "util/util_logging.h"

namespace Farm {

#define NUM_DUMMY_JOBS  512
#define NUM_DUMMY_TASKS 1024

DryRunStorage::DryRunStorage(bool populate)
    : populate_(populate) {
}

/* Perform connection to the storage. */
bool DryRunStorage::connect() {
  VLOG(1) << "Connected to the dry run storage.";
  return true;
}

/* Disconnect from the storage. */
bool DryRunStorage::disconnect() {
  VLOG(1) << "Disconnected to the dry run storage.";
  return true;
}

/* Retrieve all jobs from the storage. */
bool DryRunStorage::retrieve_all_jobs(vector<Job*> *all_jobs) {
  all_jobs->clear();
  if(populate_) {
    for(int i = 0; i < NUM_DUMMY_JOBS; ++i) {
      string name = string_printf("Job %d", i);
      Job *new_job = new Job(i, 50, Job::STATUS_WAITING, name);
      all_jobs->push_back(new_job);
    }
  }
  return true;
}

/* Retrieve all tasks of a given job from the storage. */
bool DryRunStorage::retrieve_all_tasks(const Job& job,
                                       vector<Task*> *all_tasks) {
  all_tasks->clear();
  for(int i = 0; i < NUM_DUMMY_TASKS; ++i) {
    Task *task = new Task(job.id() * NUM_DUMMY_TASKS + i,
                          Task::STATUS_WAITING);
    all_tasks->push_back(task);
  }
  return true;
}

/* Insert new job into the database. */
bool DryRunStorage::insert_job(Job * /*job*/) {
  return true;
}

/* Update job in the stroage. */
bool DryRunStorage::update_job(const Job& /*job*/) {
  return true;
}

/* Update task in the stroage. */
bool DryRunStorage::update_task(const Task& /*task*/) {
  return true;
}

/* Fliush caches to the actual storage. */
bool DryRunStorage::flush_caches(bool force) {
  return true;
}

} /* namespace Farm */
