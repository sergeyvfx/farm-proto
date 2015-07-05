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

#ifndef STORAGE_H_
#define STORAGE_H_

#include "model/model_job.h"
#include "model/model_task.h"
#include "util/util_vector.h"

namespace Farm {

class Storage {
 public:
  Storage() {}

  virtual ~Storage() {}

  /* Perform connection to the storage. */
  virtual bool connect() = 0;

  /* Disconnect from the storage. */
  virtual bool disconnect() = 0;

  /* Retrieve all jobs from the storage. */
  virtual bool retrieve_all_jobs(vector<Job*> *all_jobs) = 0;

  /* Retrieve all tasks of a given job from the storage. */
  virtual bool retrieve_all_tasks(const Job& job,
                                  vector<Task*> *all_tasks) = 0;

  /* Insert new job into the database.
   * Will take care of filling Job ID in,
   */
  virtual bool insert_job(Job *job) = 0;

  /* Update job in the stroage. */
  virtual bool update_job(const Job& job) = 0;

  /* Update task in the stroage. */
  virtual bool update_task(const Task& task) = 0;

  /* Fliush caches to the actual storage. */
  virtual bool flush_caches(bool force = false) = 0;
};

} /* namespace Farm */

#endif  /* STORAGE_H_ */
