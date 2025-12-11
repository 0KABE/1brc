//
// Created by Chen, WenTao on 2025/8/22.
//

#pragma once

#include <sys/file.h>
#include <sys/mman.h>
#include <unistd.h>

#include <cassert>
#include <filesystem>
#include <span>

struct FileMemoryMap {
  int fd{-1};
  size_t size{0};
  void *address{nullptr};

  FileMemoryMap() = default;

  explicit FileMemoryMap(const std::filesystem::path &path)
      : fd(open(path.c_str(), O_RDONLY)),
        size(std::filesystem::file_size(path)),
        address(mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0)) {
    assert(std::filesystem::exists(path));
  }

  FileMemoryMap(FileMemoryMap &&other) noexcept {
    if (this != &other) {
      *this = std::move(other);
    }
  }

  FileMemoryMap &operator=(FileMemoryMap &&other) noexcept {
    FileMemoryMap empty;
    Swap(empty);
    Swap(other);
    return *this;
  }

  explicit operator std::span<const char>() const { return {static_cast<const char *>(address), size}; }

  void Swap(FileMemoryMap &other) {
    std::swap(fd, other.fd);
    std::swap(size, other.size);
    std::swap(address, other.address);
  }

  ~FileMemoryMap() {
    if (fd != -1) {
      assert(munmap(address, size) == 0);
      assert(close(fd) == 0);
    }
  }
};
