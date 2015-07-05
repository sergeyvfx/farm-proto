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

#include "model/model_job.h"

#include "model/model_task.h"
#include "storage/storage.h"
#include "util/util_foreach.h"
#include "util/util_logging.h"

namespace Farm {

Job::Job()
    : id_(-1),
      priority_(50),
      status_(STATUS_WAITING),
      name_("") {
}

Job::Job(int id,
         Priority priority,
         Status status,
         string name)
    : id_(id),
      priority_(priority),
      status_(status),
      name_(name) {
}

Job::~Job() {
  foreach(Task *task, tasks_) {
    delete task;
  }
}

bool Job::is_running() {
  return status_ == STATUS_WAITING ||
         status_ == STATUS_ACTIVE;
}

bool Job::need_always_fetch_tasks() {
  /* return is_running(); */
  /* TODO(sergey): For now we always keep all tasks in memory. */
  return true;
}

/* (Re-)generate tasks for the job. */
void Job::generate_tasks(int start_task_id) {
  tasks_.clear();
  /* TODO(sergey): Do a real thing here. */
  for(int i = 0; i < 1024; ++i) {
    Task *new_task = new Task(start_task_id, Task::STATUS_WAITING);
    tasks_.push_back(new_task);
  }
}

/* Real tasks from the storage. */
bool Job::restore_tasks(Storage *storage) {
  storage->retrieve_all_tasks(*this, &tasks_);
  VLOG(1) << "Restored " << tasks_.size()  << " task(s) for job id " << id_;
  return true;
}

/* Store all tasks to the storage. */
bool Job::store_tasks(Storage *storage) {
  return true;
}

/* Serialize the job into JSON. */
json Job::serialize_json(bool detail) {
  /* TODO(sergey): Need to fill in a real information here. */
  json manager;
  manager["name"] = "Blender Institute";
  manager["logo"] = "http://i.imgur.com/tVQSqq3s.jpg";
  json job;
  job["id"] = id_;
  job["time_remaining"] = 0;
  job["time_average"] = 0;
  job["time_total"] = 0;
  job["time_elapsed"] = 0;
  job["job_name"] = name_;
  job["percentage_done"] = name_;
  job["status"] = "waiting";
  job["creation_date"] = "Creation Time";
  job["date_edit"] = "Edit Time";
  job["priority"] = priority_;
  job["manager"] = manager;
  job["tasks_status"] = json();
  job["username"] = "Marty McFly";
  if(detail) {
    json settings;
    settings["frame_start"] = 0;
    settings["frame_end"] = 250;
    settings["chunk_size"] = 0;

    /* TODO(sergey): This guys are to be unified in flamenco.*/
    job["total_time"] = 0;
    job["average_time"] = 0;

    job["average_time_frame"] = 0;
    job["tasks"] = "";
    job["settings"] = settings;
  }
  return job;
}

}  /* namespace Farm */
