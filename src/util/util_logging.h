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

#ifndef UTIL_LOGGING_H_
#define UTIL_LOGGING_H_

#include <glog/logging.h>

namespace Farm {

/* TODO(sergey): Enable this stubs so compilation withotu glog works. */
#if 0
class StubStream : public std::ostream {
 public:
  StubStream() : std::ostream(NULL) { }
};

class LogMessageVoidify {
 public:
  LogMessageVoidify() { }
  void operator&(::std::ostream&) { }
};

#  define LOG_SUPPRESS() (true) ? (void) 0 : LogMessageVoidify() & StubStream()
#  define LOG(severity) LOG_SUPPRESS()
#  define VLOG(severity) LOG_SUPPRESS()
#endif

void util_logging_init(const char *argv0);
void util_logging_start(void);
void util_logging_verbosity_set(int verbosity);

} /* namespace Farm */

#endif  /* UTIL_LOGGING_H_ */
