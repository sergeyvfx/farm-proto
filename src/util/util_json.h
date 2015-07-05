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

#ifndef UTIL_JSON_H_
#define UTIL_JSON_H_

#include "util/util_map.h"
#include "util_string.h"

namespace Farm {

class json;

class json_value {
 public:
  enum Type {
    UNKNOWN,
    STRING,
    INTEGER,
    FLOAT,
    JSON
  };
  json_value();
  json_value(const json_value& other);
  json_value(string value);
  json_value(int value);
  json_value(double value);
  json_value(const json& value);
  ~json_value();

  void reset();
  string serialize();

  json_value& operator= (const json_value& other);
  json_value& operator= (string value);
  json_value& operator= (int value);
  json_value& operator= (double value);
  json_value& operator= (const json& value);
 protected:
  Type type_;
  string value_string_;
  int value_integer_;
  double value_float_;
  json *value_json_;
};

class json {
 public:
  json();
  json(const json& other);
  json_value& operator[] (int key);
  json_value& operator[] (string key);
  string serialize();
 protected:
  typedef unordered_map<string, json_value> storage_type;
  storage_type storage_;
};

} /* namespace Farm */

#endif  /* UTIL_JSON_H_ */
