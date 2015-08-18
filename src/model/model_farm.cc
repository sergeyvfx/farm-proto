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

#include "model/model_farm.h"

#include "model/model_job.h"
#include "storage/storage.h"
#include "util/util_foreach.h"
#include "util/util_logging.h"

namespace Farm {

Farm::Farm(Storage *storage)
    : storage_(storage),
      max_job_id_(-1) {
}

Farm::~Farm() {
  foreach(Job *job, jobs_) {
    delete job;
  }
}

/* Real all farm data from the storage. */
bool Farm::restore() {
  VLOG(1) << "Restoring farm from the storage.";
  jobs_.clear();
  /* TODO(sergey): Proper error handling. */
  storage_->retrieve_all_jobs(&jobs_);
  VLOG(1) << "Restored " << jobs_.size() << " job(s).";
  foreach(Job *job, jobs_) {
    if(job->need_always_fetch_tasks()) {
      job->restore_tasks(storage_);
    }
  }
  rebuild_priority_queue();
  return true;
}

/* Store all the pending data to the storage. */
bool Farm::store() {
  VLOG(1) << "Flushing farm data to the storage.";
  return true;
}
/* Rebuild priority queue of tasks. */
void Farm::rebuild_priority_queue() {
  thread_scoped_lock(lock);
  VLOG(1) << "Rebuilding priority queue of tasks.";
  foreach(Job *job, jobs_) {
    if(job->is_running()) {
      foreach(Task *task, job->tasks()) {
        if(task->status() == Task::STATUS_WAITING) {
          QueueTask queue_task(job, task);
          tasks_queue_.push(queue_task);
        }
      }
    }
  }
  VLOG(1) << "Added " << tasks_queue_.size() << " elements to the queue.";
}

/* Insert new job into the farm. */
Job *Farm::insert_job(Job::Priority priority,
                      Job::Status status,
                      string name) {
  thread_scoped_lock(lock);
  ++max_job_id_;
  Job *new_job = new Job(-1,
                         priority,
                         status,
                         name);
  jobs_.push_back(new_job);
  new_job->generate_tasks(1);
  storage_->insert_job(new_job);
  /* TODO(sergey): Find more proper place for this. */
  {
    vector<Task*> tasks = new_job->tasks();
    foreach(Task *task, tasks) {
      QueueTask queue_task(new_job, task);
      tasks_queue_.push(queue_task);
    }
  }
  return new_job;
}

Task* Farm::dispatch_task() {
  thread_scoped_lock(lock);
  if(tasks_queue_.empty()) {
    return NULL;
  }
  QueueTask queue_task = tasks_queue_.top();
  VLOG(1) << "Pop queue task, job id " << queue_task.job()->id()
          << ", task id " << queue_task.task()->id() << ".";
  tasks_queue_.pop();
  Job *job = queue_task.job();
  Task *task = queue_task.task();
  task->set_status(Task::STATUS_ACTIVE);
  storage_->update_task(*task);
  if(job->status() != Job::STATUS_ACTIVE) {
    job->set_status(Job::STATUS_ACTIVE);
    storage_->update_job(*job);
  }
  return task;
}

void Farm::idle_handler() {
  thread_scoped_lock(lock);
  storage_->flush_caches();
}

Job* Farm::job_by_id(int id) {
  /* TODO(sergey): Use binary search. */
  foreach(Job* job, jobs_) {
    if(job->id() == id) {
      return job;
    }
  }
  return NULL;
}

/* Priority queue helpers. */

Farm::QueueTask::QueueTask(Job *job, Task *task)
    : job_(job),
      task_(task) {
}

bool Farm::QueueTaskPriorityCompare::operator() (
    const Farm::QueueTask& left,
    const Farm::QueueTask& right) {
  if(left.job()->priority() < right.job()->priority()) {
    return false;
  }
  if(left.job()->id() < right.job()->id()) {
    return false;
  }
  if(left.job()->priority() == right.job()->priority() &&
     left.job()->id() == right.job()->id() &&
     left.task()->id() < right.task()->id()) {
    return false;
  }
  return true;
}

}  /* namespace Farm */
