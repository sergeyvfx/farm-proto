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

#include "util/util_logging.h"

#include <cstdio>
#ifdef _MSC_VER
#  define snprintf _snprintf
#endif

namespace Farm {

void util_logging_init(const char *argv0) {
  using FARM_GFLAGS_NAMESPACE::SetCommandLineOption;

  /* Make it so FATAL messages are always print into console. */
  char severity_fatal[32];
  snprintf(severity_fatal, sizeof(severity_fatal), "%d",
           google::GLOG_FATAL);

  google::InitGoogleLogging(argv0);
  SetCommandLineOption("logtostderr", "1");
  SetCommandLineOption("v", "0");
  SetCommandLineOption("stderrthreshold", severity_fatal);
  SetCommandLineOption("minloglevel", severity_fatal);
}

void util_logging_start(void) {
  using FARM_GFLAGS_NAMESPACE::SetCommandLineOption;
  SetCommandLineOption("logtostderr", "1");
  SetCommandLineOption("v", "2");
  SetCommandLineOption("stderrthreshold", "1");
  SetCommandLineOption("minloglevel", "0");
}

void util_logging_verbosity_set(int verbosity) {
  using FARM_GFLAGS_NAMESPACE::SetCommandLineOption;
  char val[10];
  snprintf(val, sizeof(val), "%d", verbosity);
  SetCommandLineOption("v", val);
}

}  /* namespace Farm */
