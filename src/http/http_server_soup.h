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

#ifndef HTTP_SERVER_SOUP_
#define HTTP_SERVER_SOUP_

#include "http/http_server.h"

struct _SoupServer;
struct _GMainLoop;

namespace Farm {

class Farm;

class SOUPHTTPServer : public HTTPServer {
 public:
  SOUPHTTPServer(Farm *farm,
                 int port,
                 string document_root);

  ~SOUPHTTPServer();

  /* Start serving loop. */
  void start_serve();

  /* Stop serving HTTP. */
  void stop_serve();

 protected:
  _SoupServer *server_;
  _GMainLoop *main_loop_;
};

}  /* namespace Farm */

#endif  /* HTTP_SERVER_SOUP_ */
