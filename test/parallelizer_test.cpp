//
// Created by Chen, WenTao on 2025/12/11.
//

#include "parallelizer.h"

#include <fmt/format.h>
#include <gtest/gtest.h>

#include "env.h"
#include "file_memory_map.h"

TEST(Parallelizer, Base) {
  const FileMemoryMap file(Env::Instance().input_file);
  Parallelizer parallelizer(Env::Instance().threads, 1 << 21, static_cast<std::span<const char>>(file));
  const auto result = parallelizer.Process();
  EXPECT_GT(result.size(), 0);
}
