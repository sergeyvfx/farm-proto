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

#ifndef DRYRUN_STORAGE_H_
#define DRYRUN_STORAGE_H_

#include "storage/storage.h"

namespace Farm {

class DryRunStorage : public Storage {
 public:
  explicit DryRunStorage(bool populate);

  /* Perform connection to the storage. */
  bool connect();

  /* Disconnect from the storage. */
  bool disconnect();

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
 public:
  /* Populate the storage with test jobs/tasls. */
  bool populate_;
};

} /* namespace Farm */

#endif  /* DRYRUN_STORAGE_DATABASE_H_ */
