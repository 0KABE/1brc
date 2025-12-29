//
// Created by Chen, WenTao on 2025/11/2.
//

#include <fmt/format.h>

#include "env.h"
#include "file_memory_map.h"
#include "parallelizer.h"

class Reader {};

int main() {
  const FileMemoryMap file(Env::Instance().input_file);
  Parallelizer parallelizer(Env::Instance().threads, 1 << 21, static_cast<std::span<const char>>(file));
  const auto result = parallelizer.Process();

  for (const auto& [key, value] : result) {
    fmt::print("{}={}/{}/{} ", key.name, value.min / 10.0, value.max / 10.0, value.sum / value.count / 10);
  }

  return 0;
}
