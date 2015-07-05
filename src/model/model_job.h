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

#ifndef MODEL_JOB_
#define MODEL_JOB_

#include "util/util_json.h"
#include "util/util_string.h"
#include "util/util_vector.h"

namespace Farm {

class Storage;
class Task;

class Job {
 public:
  typedef unsigned char Priority;

  enum Status {
    STATUS_WAITING = 0,
    STATUS_ACTIVE,
    STATUS_COMPLETED,
    STATUS_CANCELLED,
    STATUS_PAUSED,
  };

  Job();

  Job(int id,
      Priority priority,
      Status status,
      string name);

  ~Job();

  inline int id() const { return id_; }
  inline void set_id(int id) { id_ = id; }
  inline int priority() const { return priority_; }
  inline void set_priority(Priority priority) { priority_ = priority; }
  inline int status() const { return status_; }
  inline void set_status(Status status) { status_ = status; }
  inline const string& name() const { return name_; }
  inline void set_name(string name) { name_ = name; }
  inline vector<Task*>& tasks() { return tasks_; }

  /* Check whether the job is stll running. */
  bool is_running();

  /* Check whetehr tasks are to be always fetched.
   *
   * Mainly so active jobs will store their tasks in memory,
   * inactive ones will keep them in the database.
   */
  bool need_always_fetch_tasks();

  /* (Re-)generate tasks for the job. */
  void generate_tasks(int start_task_id);

  /* Real tasks from the storage. */
  bool restore_tasks(Storage *storage);

  /* Store all tasks to the storage. */
  bool store_tasks(Storage *storage);

  /* Serialize the job into JSON. */
  json serialize_json(bool detail = false);
 protected:
  /* Unique ID of the job. */
  int id_;
  /* Priority of the job. */
  Priority priority_;
  /* Status of the job. */
  Status status_;
  /* Name of the job. */
  string name_;
  /* Tasks of the job. */
  vector<Task*> tasks_;
};

}  /* namespace Farm */

#endif  /* MODEL_JOB_ */
