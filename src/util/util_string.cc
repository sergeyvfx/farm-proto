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

#include "util/util_string.h"

#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <vector>

#ifdef _WIN32
#  ifndef vsnprintf
#    define vsnprintf _vsnprintf
#  endif
#endif

namespace Farm {

/* Based on code from Cycles. */
string string_printf(const char *format, ...) {
  std::vector<char> str(128, 0);
  while(1) {
    va_list args;
    int result;
    va_start(args, format);
    result = vsnprintf(&str[0], str.size(), format, args);
    va_end(args);
    if(result == -1) {
      /* Not enough space or formatting error. */
      if(str.size() > 65536) {
        assert(0);
        return string("");
      }
      str.resize(str.size()*2, 0);
      continue;
    } else if(result >= (int)str.size()) {
      /* Not enough space. */
      str.resize(result + 1, 0);
      continue;
    }
    return string(&str[0]);
  }
}

} /* namespace Farm */
