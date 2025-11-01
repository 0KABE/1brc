//
// Created by Chen, WenTao on 2025/9/9.
//

#include <benchmark/benchmark.h>

#include "parallel/memory_contiguous_allocator.h"
#include "parallel/mutex_free_worker_v1.h"
#include "utils/env.h"

class ContiguousAllocatorFixture : public benchmark::Fixture {
 public:
  void SetUp(const benchmark::State &) override {
    const Env &env = Env::Instance();
    memory_map = FileMemoryMap(env.input_file);
    allocator = ContiguousAllocator<true>(static_cast<ContiguousAllocator<true>::Buffer>(memory_map));
  }

 protected:
  FileMemoryMap memory_map;
  ContiguousAllocator<true> allocator;
};

BENCHMARK_F(ContiguousAllocatorFixture, Speed)(benchmark::State &state) {
  size_t size{0};
  for (auto _ : state) {
    if (allocator.Empty()) break;

    auto span = allocator.Allocate(1 << 22);  // 10MB
    for (auto c : span) {
      benchmark::DoNotOptimize(c);
    }
    size += span.size();
  }
  // std::println("thread: {}, size: {}", state.thread_index(), size);
  state.SetBytesProcessed(size);
}

BENCHMARK_REGISTER_F(ContiguousAllocatorFixture, Speed)
    ->UseRealTime()  // 对于多线程测试，使用真实时间（Wall-clock time）通常更有意义
    ->ThreadRange(1, std::thread::hardware_concurrency() * 4);
