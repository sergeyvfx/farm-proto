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

#include "http/http_server_soup.h"

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <libsoup/soup.h>
#include <sys/stat.h>

#include "model/model_farm.h"
#include "model/model_task.h"
#include "util/util_logging.h"
#include "util/util_path.h"
#include "util/util_string.h"
#include "util/util_time.h"

namespace Farm {

namespace {

void serve_callback_begin_log(SoupMessage *msg,
                              const char *path,
                              const char *handler) {
  VLOG(1) << "New request, method=" << msg->method
          << ", path=" << path
          << ", HTTP/1." << soup_message_get_http_version(msg)
          << ", handled by " << handler << ".";

  /* TODO(sergey): Don't iterate if verbosity level is not high enough. */
  SoupMessageHeadersIter iter;
  const char *name, *value;
  soup_message_headers_iter_init(&iter, msg->request_headers);
  while(soup_message_headers_iter_next(&iter, &name, &value)) {
    VLOG(2) << name << " " << value;
  }
}

void serve_callback_end_log(SoupMessage *msg) {
  VLOG(1) << "Handled request, "
          << msg->status_code << " " << msg->reason_phrase << ".";
}

void perform_file_serve(SoupServer *server,
                        SoupMessage *msg,
                        const string& path) {
  struct stat st;
  if(stat(path.c_str(), &st) == -1) {
    if(errno == EPERM)
      soup_message_set_status(msg, SOUP_STATUS_FORBIDDEN);
    else if(errno == ENOENT)
      soup_message_set_status(msg, SOUP_STATUS_NOT_FOUND);
    else
      soup_message_set_status(msg, SOUP_STATUS_INTERNAL_SERVER_ERROR);
    return;
  }

  if(S_ISDIR(st.st_mode)) {
    /* We don't serve directories so far. */
    soup_message_set_status(msg, SOUP_STATUS_FORBIDDEN);
    return;
  }

  if(msg->method == SOUP_METHOD_GET) {
    GMappedFile *mapping = g_mapped_file_new(path.c_str(), FALSE, NULL);
    if(mapping == NULL) {
      soup_message_set_status(msg, SOUP_STATUS_INTERNAL_SERVER_ERROR);
      return;
    }
    SoupBuffer *buffer =
      soup_buffer_new_with_owner(g_mapped_file_get_contents(mapping),
                                 g_mapped_file_get_length(mapping),
                                 mapping,
                                 (GDestroyNotify)g_mapped_file_unref);
    soup_message_body_append_buffer(msg->response_body, buffer);
    soup_buffer_free(buffer);
  } else /* if(msg->method == SOUP_METHOD_HEAD) */ {
    string length = string_printf("%lu", (gulong)st.st_size);
    soup_message_headers_append(msg->response_headers,
                                "Content-Length", length.c_str());
  }
  soup_message_set_status(msg, SOUP_STATUS_OK);
}

void serve_static_callback(SoupServer *server,
                           SoupMessage *msg,
                           const char *path,
                           GHashTable *query,
                           SoupClientContext *context,
                           gpointer data) {
  serve_callback_begin_log(msg, path, __func__);
  SOUPHTTPServer *http_server = (SOUPHTTPServer*)data;
  /* TODO(sergey): Check the path is not goig outside of the document root. */
  string file_path = path_join(http_server->get_document_root(), path);
  if(msg->method == SOUP_METHOD_GET || msg->method == SOUP_METHOD_HEAD) {
    perform_file_serve(server, msg, file_path);
  } else {
    soup_message_set_status(msg, SOUP_STATUS_FORBIDDEN);
  }
  serve_callback_end_log(msg);
}

void serve_welcome_callback(SoupServer *server,
                            SoupMessage *msg,
                            const char *path,
                            GHashTable *query,
                            SoupClientContext *context,
                            gpointer data) {
  serve_callback_begin_log(msg, path, __func__);
  if(msg->method == SOUP_METHOD_GET) {
    if(strcmp(path, "/") != 0) {
      soup_message_set_status(msg, SOUP_STATUS_NOT_FOUND);
    } else {
      const char *stub = "Welcome to the farm.";
      soup_message_set_response(msg,
                                "text/html",
                                SOUP_MEMORY_STATIC,
                                stub,
                                strlen(stub));
      soup_message_set_status(msg, SOUP_STATUS_OK);
    }
  } else {
    soup_message_set_status(msg, SOUP_STATUS_FORBIDDEN);
  }
  serve_callback_end_log(msg);
}

void serve_get_task_callback(SoupServer *server,
                             SoupMessage *msg,
                             const char *path,
                             GHashTable *query,
                             SoupClientContext *context,
                             gpointer data) {
  serve_callback_begin_log(msg, path, __func__);
  if(msg->method == SOUP_METHOD_GET) {
    SOUPHTTPServer *http_server = (SOUPHTTPServer*)data;
    Task *task = http_server->get_farm()->dispatch_task();
    if(task != NULL) {
      VLOG(1) << "Found new task for dispatch: " << task->id() << ".";
      string task_id = string_printf("%d", task->id());
      soup_message_set_response(msg,
                                "text/html",
                                SOUP_MEMORY_COPY,
                                task_id.c_str(),
                                task_id.size());
      soup_message_set_status(msg, SOUP_STATUS_OK);
    } else {
      VLOG(1) << "No tasks found for dispatch,";
      soup_message_set_status(msg, SOUP_STATUS_NOT_FOUND);
    }
  } else {
    soup_message_set_status(msg, SOUP_STATUS_FORBIDDEN);
  }
  serve_callback_end_log(msg);
}

gboolean idle_function(gpointer user_data) {
  SOUPHTTPServer *http_server = (SOUPHTTPServer*)user_data;
  if(http_server->idle_function_cb) {
    http_server->idle_function_cb();
  }
  /* TODO(sergey): Might need to tweak this delay.
   * or maybe use timer callback instead.
   */
  util_time_sleep(0.01);
  return true;
}

}  /* namespace */

SOUPHTTPServer::SOUPHTTPServer(Farm *farm,
                               int port,
                               string document_root)
    : HTTPServer(farm, port, document_root),
      main_loop_(NULL) {
  VLOG(1) << "Create new SOUP HTTP server ar port " << port << ".";
  server_ = soup_server_new(SOUP_SERVER_SERVER_HEADER, "farm-httpd", NULL);
}

SOUPHTTPServer::~SOUPHTTPServer() {
  g_object_unref(server_);
}

void SOUPHTTPServer::start_serve() {
  SoupServerListenOptions listen_options = SOUP_SERVER_LISTEN_IPV4_ONLY;
  GError *error = NULL;

  /* TODO(sergey): Implement proper error handling. */
  soup_server_listen_all(server_, 8080, listen_options, &error);

  /* Define static routes. */
  soup_server_add_handler(server_, "/", serve_welcome_callback, this, NULL);
  soup_server_add_handler(server_, "/static", serve_static_callback, this, NULL);
  soup_server_add_handler(server_,
                          "/get_task",
                          serve_get_task_callback,
                          this,
                          NULL);

  GSList *uris, *u;
  uris = soup_server_get_uris(server_);
  for(u = uris; u; u = u->next) {
    char *str = soup_uri_to_string((SoupURI*)u->data, FALSE);
    VLOG(1) << "Listening on " << str << ".";
    g_free(str);
    soup_uri_free((SoupURI*)u->data);
  }
  g_slist_free(uris);

  /* TODO(sergey): It'll work for until more areas will want
   * to use glib's main loop.
   */
  main_loop_ = g_main_loop_new(NULL, FALSE);

  g_idle_add(idle_function, this);

  g_main_loop_run(main_loop_);
}

void SOUPHTTPServer::stop_serve() {
  VLOG(1) << "Stop listening to HTTP connections.";
  g_main_loop_quit(main_loop_);
  soup_server_disconnect(server_);
}

}  /* namespace Farm */
