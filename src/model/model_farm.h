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

#ifndef MODEL_FARM_
#define MODEL_FARM_

#include "model/model_job.h"

#include "util/util_priority_queue.h"
#include "util/util_thread.h"
#include "util/util_vector.h"

namespace Farm {

class Storage;
class Task;

class Farm {
 public:
  explicit Farm(Storage *storage);

  ~Farm();

  /* Real all farm data from the storage. */
  bool restore();

  /* Store all the pending data to the storage. */
  bool store();

  /* Insert new job into the farm. */
  Job *insert_job(Job::Priority priority,
                  Job::Status status,
                  string name);

  /* Dispatch new task to worker/manager. */
  Task* dispatch_task();

  /* Does all the maintenance work while http server is idling. */
  void idle_handler();

  /* Getters */
  vector<Job*>& jobs() { return jobs_; }
  Job* job_by_id(int id);
 protected:
  class QueueTask {
   public:
    QueueTask(Job *job, Task *task);
    bool operator <(const QueueTask& other) const;
    Job *job() const { return job_; }
    Task *task() const { return task_; }
   protected:
    Job *job_;
    Task *task_;
  };
  class QueueTaskPriorityCompare {
    public:
      bool operator() (const QueueTask& left, const QueueTask& righr);
  };

  /* Rebuild priority queue of tasks. */
  void rebuild_priority_queue();

  /* Descriptor used to communicate with the storage. */
  Storage *storage_;
  /* Jobs registered in the farm. */
  vector<Job*> jobs_;
  /* Max job ID used for indexing.
   *
   * This way we're getting rid of need of AUTOINCREMENT fields
   * which are slow in certain database servers and which might
   * not exist in other storage types.
   */
  int max_job_id_;
  /* Queue of the tasks, used for faster dispatching. */
  priority_queue<QueueTask,
                 vector<QueueTask>,
                 QueueTaskPriorityCompare> tasks_queue_;
  /* Mutex lock used for threading critical operations. */
  thread_mutex lock;
};

}  /* namespace Farm */

#endif  /* MODEL_FARM_ */
