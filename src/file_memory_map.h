//
// Created by Chen, WenTao on 2025/8/22.
//

#pragma once

#include <filesystem>
#include <sys/file.h>
#include <sys/mman.h>

#include <cassert>
#include <unistd.h>

struct FileMemoryMap {
  const int fd;
  const size_t size;
  void *const address;

  explicit FileMemoryMap(const std::filesystem::path &path)
      : fd(open(path.c_str(), O_RDONLY)),
        size(std::filesystem::file_size(path)),
        address(mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0)) {
    assert(std::filesystem::exists(path));
  }

  explicit operator std::span<const char>() const {
    return {static_cast<const char *>(address), size};
  }

  ~FileMemoryMap() {
    assert(munmap(address, size) == 0);
    assert(close(fd) == 0);
  }
};
