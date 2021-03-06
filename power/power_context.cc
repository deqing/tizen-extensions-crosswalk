// Copyright (c) 2013 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "power/power_context.h"
#include "common/picojson.h"

DEFINE_XWALK_EXTENSION(PowerContext);

PowerContext::PowerContext(ContextAPI* api)
  : api_(api) {
  PlatformInitialize();
}

PowerContext::~PowerContext() {
  PlatformUninitialize();
}

const char PowerContext::name[] = "tizen.power";

// This will be generated from power_api.js.
extern const char kSource_power_api[];

const char* PowerContext::GetJavaScript() {
  return kSource_power_api;
}

void PowerContext::HandleMessage(const char* message) {
  picojson::value v;

  std::string err;
  picojson::parse(v, message, message + strlen(message), &err);
  if (!err.empty()) {
    std::cout << "Ignoring message.\n";
    return;
  }

  std::string cmd = v.get("cmd").to_str();
  if (cmd == "PowerRequest") {
    HandleRequest(v);
  } else if (cmd == "PowerRelease") {
    HandleRelease(v);
  } else if (cmd == "PowerSetScreenBrightness") {
    // value of -1 means restore to default value.
    HandleSetScreenBrightness(v);
  } else if (cmd == "PowerSetScreenEnabled") {
    HandleSetScreenEnabled(v);
  } else {
    std::cout << "ASSERT NOT REACHED.\n";
  }
}

void PowerContext::HandleSyncMessage(const char* message) {
  picojson::value v;

  std::string err;
  picojson::parse(v, message, message + strlen(message), &err);
  if (!err.empty()) {
    std::cout << "Ignoring message.\n";
    return;
  }

  std::string cmd = v.get("cmd").to_str();
  if (cmd == "PowerGetScreenBrightness") {
    HandleGetScreenBrightness();
  }  else if (cmd == "PowerGetScreenState") {
    HandleGetScreenState();
  } else {
    std::cout << "ASSERT NOT REACHED.\n";
  }
}

void PowerContext::OnScreenStateChanged(ResourceState state) {
  picojson::value::object o;
  o["cmd"] = picojson::value("ScreenStateChanged");
  o["state"] = picojson::value(static_cast<double>(state));

  picojson::value v(o);
  api_->PostMessage(v.serialize().c_str());
}
