//
// Created by Chen, WenTao on 2025/11/3.
//

#pragma once
#include <cassert>
#include <span>

#include "data.h"

Entity ParseOnce_Base(std::span<const char> &span);

Entity ParseOnceV1(std::span<const char> &span);

uint64_t FilterZeroByte(uint64_t x);
