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

#include <cstdlib>
#include <libsoup/soup.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "util/util_logging.h"
#include "util/util_time.h"

namespace Farm {

int main(int argc, char **argv) {
  util_logging_init(argv[0]);
  /* TODO(sergey): Make it a ocmmand line argument. */
  util_logging_start();
  util_logging_verbosity_set(1);

  SoupSession *session = soup_session_new_with_options(
      SOUP_SESSION_ADD_FEATURE_BY_TYPE, SOUP_TYPE_CONTENT_SNIFFER,
      NULL);

  SoupMessage *msg = soup_message_new("GET", "http://127.0.0.1:9999/get_task");

  int num_tasks_handled = 0;
  double start_time;
  for(;;) {
    guint status = soup_session_send_message(session, msg);
    if(status == 200) {
      if(num_tasks_handled == 0) {
        start_time = util_time_dt();
      }
      ++num_tasks_handled;
      continue;
    } else if (status == 404) {
      VLOG(1) << "All done!.";
      break;
    }
    if(num_tasks_handled != 0) {
      VLOG(1) << "Number of handled tasks: " << num_tasks_handled << ".";
      VLOG(1) << "Tasks per second: "
              << (double)num_tasks_handled / (util_time_dt() - start_time)
              << ".";
      num_tasks_handled = 0;
    }
    VLOG(1) << "Wait for server to come back.";
    sleep(2);
  }
  VLOG(1) << "Number of handled tasks: " << num_tasks_handled << ".";
  VLOG(1) << "Tasks per second: "
          << (double)num_tasks_handled / (util_time_dt() - start_time)
          << ".";

  return EXIT_SUCCESS;
}

}  /* namespace Farm */

int main(int argc, char **argv) {
  return Farm::main(argc, argv);
}
