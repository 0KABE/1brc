//
// Created by Chen, WenTao on 2025/8/19.
//

#pragma once

#include <string>

struct Env {
  constexpr static auto kThread = "THREADS";
  constexpr static auto kInputFile = "INPUT_FILE";

  std::string input_file{getenv(kInputFile)};
  int threads{std::stoi(getenv(kThread) ? getenv(kThread) : "1")};

  static const Env &Instance() {
    static Env instance;
    return instance;
  }
};
