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

#include "util/util_foreach.h"
#include "util/util_json.h"

namespace Farm {

json_value::json_value()
  : type_(UNKNOWN),
    value_string_(""),
    value_integer_(0),
    value_float_(0),
    value_json_(NULL) {
}

json_value::json_value(const json_value& other) {
  type_ = other.type_;
  value_string_ = other.value_string_;
  value_integer_ = other.value_integer_;
  value_float_ = other.value_float_;
  if(other.value_json_) {
    value_json_ = new json(*other.value_json_);
  }
  else {
    value_json_ = NULL;
  }
}

json_value::json_value(string value)
  : type_(STRING),
    value_string_(value),
    value_integer_(0),
    value_float_(0),
    value_json_(NULL) {
}

json_value::json_value(int value)
  : type_(INTEGER),
    value_string_(""),
    value_integer_(value),
    value_float_(0),
    value_json_(NULL) {
}

json_value::json_value(double value)
  : type_(FLOAT),
    value_string_(""),
    value_integer_(0),
    value_float_(value),
    value_json_(NULL) {
}

json_value::json_value(const json& value)
  : type_(JSON),
    value_string_(""),
    value_integer_(0),
    value_float_(0) {
  value_json_ = new json(value);
}

json_value::~json_value() {
  if(type_ == JSON) {
    delete value_json_;
  }
}

void json_value::reset() {
  if(type_ == JSON) {
    delete value_json_;
  }
  type_ = UNKNOWN;
  value_string_.clear();
  value_integer_ = 0;
  value_float_ = 0.0;
  value_json_ = NULL;
}

string json_value::serialize() {
  switch(type_) {
    case STRING:
      return "\"" + value_string_ + "\"";
    case INTEGER:
      return string_printf("%d", value_integer_);
    case FLOAT:
      return string_printf("%f", value_float_);
    case JSON:
      return value_json_->serialize();
    default:
      return "";
  }
}

json_value& json_value::operator= (string value) {
  reset();
  type_ = STRING;
  value_string_ = value;
  return *this;
}

json_value& json_value::operator= (int value) {
  reset();
  type_ = INTEGER;
  value_integer_ = value;
  return *this;
}

json_value& json_value::operator= (double value) {
  reset();
  type_ = FLOAT;
  value_float_ = value;
  return *this;
}

json_value& json_value::operator= (const json& value) {
  reset();
  type_ = JSON;
  value_json_ = new json(value);
  return *this;
}

/* JSON serialization/deserialization */

json::json() {
}

json::json(const json& other) {
  storage_ = other.storage_;
}

json_value& json::operator[] (int key) {
  string key_string = string_printf("%d", key);
  return storage_[key_string];
}

json_value& json::operator[] (string key) {
  return storage_[key];
}

string json::serialize() {
  string result = "{";
  bool first_time = true;
  foreach(storage_type::value_type& pair, storage_) {
    if(!first_time) {
      result += ",";
    }
    result += string_printf(" \"%s\": %s",
                            pair.first.c_str(),
                            pair.second.serialize().c_str());
    first_time = false;
  }
  result += "}";
  return result;
}

} /* namespace Farm */
