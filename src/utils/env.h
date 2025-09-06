//
// Created by Chen, WenTao on 2025/8/19.
//

#pragma once

#include <string>

struct Env {
  std::string input_file;

  Env() : input_file(getenv("INPUT_FILE")) {}

  static const Env &Instance() {
    static Env instance;
    return instance;
  }
};