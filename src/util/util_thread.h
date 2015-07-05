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

#ifndef UTIL_THREAD_H_
#define UTIL_THREAD_H_

#if (__cplusplus > 199711L) || (defined(_MSC_VER) && _MSC_VER >= 1800)
#  include <thread>
#  include <mutex>
#  include <condition_variable>
#else
#  include <boost/thread.hpp>
#endif

namespace Farm {

#if (__cplusplus > 199711L) || (defined(_MSC_VER) && _MSC_VER >= 1800)
typedef std::mutex thread_mutex;
typedef std::unique_lock<std::mutex> thread_scoped_lock;
typedef std::condition_variable thread_condition_variable;
#else
typedef boost::mutex thread_mutex;
typedef boost::mutex::scoped_lock thread_scoped_lock;
typedef boost::condition_variable thread_condition_variable;
#endif

}  /* namespace Farm */

#endif  /* UTIL_THREAD_H_ */
