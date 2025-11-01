//
// Created by Chen, WenTao on 2025/9/6.
//
#include <benchmark/benchmark.h>

#include "parallel/memory_contiguous_allocator.h"
#include "parallel/mutex_free_contiguous_allocator.h"
#include "parallel/mutex_free_worker_v1.h"
#include "utils/env.h"

int main() {
  const Env &env = Env::Instance();
  const auto memory_map = FileMemoryMap(env.input_file);
  auto allocator = ContiguousAllocator<true>(static_cast<ContiguousAllocator<true>::Buffer>(memory_map));

  std::vector<std::thread> threads;
  std::atomic_size_t file_size{0};

  for (auto i = 0; i < env.threads; ++i) {
    threads.emplace_back([&] {
      size_t size = 0;
      while (!allocator.Empty()) {
        auto buffer = allocator.Allocate(1 << 24);
        for (auto c : buffer) {
          benchmark::DoNotOptimize(c);
        }
        size += buffer.size();
      }
      file_size += size;
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }

  std::println("thread: {}, file size: {}", env.threads, file_size.load());
}
