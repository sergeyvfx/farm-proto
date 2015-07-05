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

#ifndef UTIL_TIME_H_
#define UTIL_TIME_H_

#include <sys/time.h>
#include <time.h>
#include <unistd.h>

namespace Farm {

double util_time_dt() {
  struct timeval now;
  gettimeofday(&now, NULL);
  return now.tv_sec + now.tv_usec*1e-6;
}

void util_time_sleep(double t) {
  struct timespec tim;
  tim.tv_sec = (int)t;
  tim.tv_nsec = (t - tim.tv_sec) * 1e+9;
  nanosleep(&tim , NULL);
}

}  /* namespace Farm */

#endif  /* UTIL_TIME_H_ */
