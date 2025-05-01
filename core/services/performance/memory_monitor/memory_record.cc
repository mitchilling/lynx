// Copyright 2025 The Lynx Authors. All rights reserved.
// Licensed under the Apache License Version 2.0 that can be found in the
// LICENSE file in the root directory of this source tree.

#include "core/services/performance/memory_monitor/memory_record.h"

namespace lynx {
namespace tasm {
namespace performance {

MemoryRecord::MemoryRecord(MemoryCategory category, float size_kb)
    : category_(std::move(category)), size_kb_(size_kb) {}

MemoryRecord::MemoryRecord(
    MemoryCategory category, float size_kb,
    std::unique_ptr<std::unordered_map<std::string, std::string>> detail)
    : category_(std::move(category)),
      size_kb_(size_kb),
      detail_(std::move(detail)) {}

MemoryRecord& MemoryRecord::operator+=(const MemoryRecord& other) {
  size_kb_ += other.size_kb_;
  // Merge detail_
  if (!other.detail_) {
    return *this;
  }
  if (!detail_) {
    // If we have no detail, create a copy of other's
    detail_ = std::make_unique<std::unordered_map<std::string, std::string>>(
        *other.detail_);
    return *this;
  }
  for (const auto& [key, value] : *other.detail_) {
    (*detail_)[key] = value;
  }
  return *this;
}

MemoryRecord& MemoryRecord::operator-=(const MemoryRecord& other) {
  size_kb_ -= other.size_kb_;
  // Deduplicate detail_
  if (!other.detail_ || !detail_) {
    return *this;
  }
  for (const auto& [key, value] : *other.detail_) {
    detail_->erase(key);
  }
  return *this;
}

}  // namespace performance
}  // namespace tasm
}  // namespace lynx
