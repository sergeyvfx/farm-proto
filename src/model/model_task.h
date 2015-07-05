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

#ifndef MODEL_TASK_
#define MODEL_TASK_

namespace Farm {

class Task {
 public:
  enum Status {
    STATUS_WAITING = 0,
    STATUS_PROCESSING,
    STATUS_ACTIVE,
    STATUS_COMPLETED,
    STATUS_FAILED,
  };

  Task();

  Task(int id, Status status);

  inline int id() const { return id_; }
  inline void set_id(int id) { id_ = id; }
  inline Status status() const { return status_; }
  inline void set_status(Status status) { status_ = status; }

  /* Check whether the task is stll running. */
  bool is_running();

 protected:
  /* Unique ID of the task. */
  int id_;
  /* Status of the task. */
  Status status_;
};

}  /* namespace Farm */

#endif  /* MODEL_TASK_ */
