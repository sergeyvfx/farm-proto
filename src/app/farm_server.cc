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
#include <csignal>

#include "http/http_server_soup.h"
#include "model/model_farm.h"
#include "storage/storage_dryrun.h"
#include "storage/storage_database_sqlite.h"
#include "util/util_function.h"
#include "util/util_string.h"
#include "util/util_logging.h"
#include "util/util_time.h"

/* Use dry-run (emory-only, no serialization) storage. */
// #define DRY_RUN_STORAGE
/* Create full bunch of new jobs, for development purposes only. */
// #define CREATE_NEW_JOBS

namespace Farm {

namespace {

Farm *farm = NULL;
HTTPServer *http_server = NULL;
Storage *storage = NULL;

void signal_quit(int sig) {
  VLOG(1) << "Performing shutdown sequence...";
  http_server->stop_serve();
}

}  /* namespace */

int main(int argc, char **argv) {
  signal(SIGINT, signal_quit);

  util_logging_init(argv[0]);
  /* TODO(sergey): Make it a ocmmand line argument. */
  util_logging_start();
  util_logging_verbosity_set(1);

#ifndef DRY_RUN_STORAGE
  // SQLiteStorage *sqlite_storage = new SQLiteStorage(":memory:");
  SQLiteStorage *sqlite_storage = new SQLiteStorage("/tmp/farm.sqlite");
  sqlite_storage->connect();
  sqlite_storage->create_schema();

  sqlite_storage->use_bulked_transactions = true;
  sqlite_storage->transaction_commit_interval = 2.0;

  storage = sqlite_storage;
#else
  storage = new DryRunStorage(true);
  storage->connect();
#endif

  double start_time = util_time_dt();
  farm = new Farm(storage);
  farm->restore();
#ifdef CREATE_NEW_JOBS
  for(int i = 0; i < 512; ++i) {
    farm->insert_job(50,
                     Job::STATUS_WAITING,
                     string_printf("Test Job %d", i));
  }
#endif
  VLOG(1) << "Restored in " << util_time_dt() - start_time << " seconds.";

  http_server = new  SOUPHTTPServer(farm,
                                    8080,
                                    "/home/sergey/src/farm-proto/web");
  http_server->idle_function_cb = function_bind(&Farm::idle_handler, farm);
  http_server->start_serve();

  farm->store();
  storage->disconnect();
  delete http_server;
  delete storage;
  delete farm;
  VLOG(1) << "Shutdown sequence completed.";

  return EXIT_SUCCESS;
}

}  /* namespace Farm */

int main(int argc, char **argv) {
  return Farm::main(argc, argv);
}
