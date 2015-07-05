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

#ifndef HTTP_SERVER_
#define HTTP_SERVER_

#include "util/util_function.h"
#include "util/util_string.h"

namespace Farm {

class Farm;

class HTTPServer {
 public:
  /* Function which is being called when HTTP server is idling. */
  function<void(void)> idle_function_cb;

  HTTPServer(Farm *farm,
             int port,
             string document_root)
    : farm_(farm),
      port_(port),
      document_root_(document_root) {
  }
  virtual ~HTTPServer() {}

  /* Start serving loop. */
  virtual void start_serve() = 0;

  /* Stop serving HTTP. */
  virtual void stop_serve() = 0;

  /* Get path to the document root, */
  const string& get_document_root() { return document_root_; }

  Farm *farm() { return farm_; }

 protected:
  /* Back-link to the farm, so http cal invoke methods from it. */
  Farm *farm_;
  /* Port number to listen on. */
  int port_;
  /* Path to the server document root. */
  string document_root_;
};

}  /* namespace Farm */

#endif  /* HTTP_SERVER_SOUP_ */
