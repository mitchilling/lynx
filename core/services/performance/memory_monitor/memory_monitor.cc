// Copyright 2025 The Lynx Authors. All rights reserved.
// Licensed under the Apache License Version 2.0 that can be found in the
// LICENSE file in the root directory of this source tree.

#include "core/services/performance/memory_monitor/memory_monitor.h"

#include <memory>
#include <utility>

#include "core/renderer/utils/lynx_env.h"
#include "third_party/rapidjson/document.h"

namespace lynx {
namespace tasm {
namespace performance {

inline MemoryRecord BuildMemoryRecord(
    const rapidjson::Value& obj,
    std::unordered_map<std::string, std::string> info) {
  MemoryRecord record;
  // size_kb
  const rapidjson::Value& heapsize_after_v = obj["heapsize_after"];
  if (heapsize_after_v.IsNumber()) {
    record.size_kb_ = heapsize_after_v.GetUint();
  }
  // category
  auto category_it = info.find(kCategory);
  if (category_it != info.end()) {
    record.category_ = category_it->second;
  }
  // detail
  record.detail_ =
      std::make_unique<std::unordered_map<std::string, std::string>>(
          std::move(info));
  if (obj.IsObject()) {
    for (rapidjson::Value::ConstMemberIterator itr = obj.MemberBegin();
         itr != obj.MemberEnd(); ++itr) {
      std::string key = itr->name.GetString();
      if (key == kRawRuntimeMemoryInfo) {
        continue;
      }
      std::string value;
      if (itr->value.IsString()) {
        value = itr->value.GetString();
      } else if (itr->value.IsInt()) {
        value = std::to_string(itr->value.GetInt());
      } else if (itr->value.IsDouble()) {
        value = std::to_string(itr->value.GetDouble());
      } else if (itr->value.IsBool()) {
        value = itr->value.GetBool() ? "true" : "false";
      } else {
        value = "Unsupported type";
      }
      record.detail_->emplace(key, std::move(value));
    }
  }
  return record;
}

static std::atomic<bool> g_force_enable_ = false;

bool MemoryMonitor::Enable() {
  return g_force_enable_ || LynxEnv::GetInstance().EnableMemoryMonitor();
}

void MemoryMonitor::SetForceEnable(bool force_enable) {
  g_force_enable_ = force_enable;
}

uint32_t MemoryMonitor::MemoryChangeThresholdMb() {
  return LynxEnv::GetInstance().GetMemoryChangeThresholdMb();
}

uint32_t MemoryMonitor::ScriptingEngineMode() {
  uint32_t mode = 0;
  bool enable_mem_monitor = Enable();
  if (!enable_mem_monitor) {
    return mode;
  }
  // 8 bit max number is 255.
  uint32_t mem_increment_threshold_mb = MemoryChangeThresholdMb();
  if (mem_increment_threshold_mb > 255) {
    mem_increment_threshold_mb = 255;
  }
  mode = (mem_increment_threshold_mb << 24);
  return mode;
}

MemoryMonitor::~MemoryMonitor() {
  if (Enable()) {
    // Clear records and report 0 memory usage.
    memory_records_.clear();
    ReportMemory();
  }
}

void MemoryMonitor::AllocateMemory(MemoryRecord&& record) {
  if (!Enable()) {
    return;
  }
  auto ret_record_it = memory_records_.find(record.category_);
  if (ret_record_it == memory_records_.end()) {
    memory_records_.emplace(record.category_, std::move(record));
  } else {
    ret_record_it->second += record;
  }
  ReportMemory();
}

void MemoryMonitor::DeallocateMemory(MemoryRecord&& record) {
  if (!Enable()) {
    return;
  }
  auto ret_record_it = memory_records_.find(record.category_);
  if (ret_record_it == memory_records_.end()) {
    return;
  }
  ret_record_it->second -= record;
  ReportMemory();
}

void MemoryMonitor::UpdateMemoryUsage(MemoryRecord&& record) {
  if (!Enable()) {
    return;
  }
  memory_records_[record.category_] = std::move(record);
  ReportMemory();
}

void MemoryMonitor::UpdateScriptingEngineMemoryUsage(
    std::unordered_map<std::string, std::string> info) {
  if (!Enable()) {
    return;
  }
  auto it = info.find(kRawRuntimeMemoryInfo);
  if (it == info.end()) {
    return;
  }

  rapidjson::Document doc;
  doc.Parse(it->second);
  info.erase(kRawRuntimeMemoryInfo);
  if (doc.HasParseError() || !doc.IsObject()) {
    return;
  }
  const rapidjson::Value& gc_info = doc["gc_info"];
  if (!gc_info.IsArray()) {
    return;
  }
  rapidjson::SizeType arraySize = gc_info.Size();
  if (arraySize == 0) {
    return;
  }
  const rapidjson::Value& lastElement = gc_info[arraySize - 1];
  if (lastElement.IsNull()) {
    return;
  }
  UpdateMemoryUsage(BuildMemoryRecord(lastElement, std::move(info)));
}

void MemoryMonitor::ReportMemory() {
  if (sender_ == nullptr) {
    return;
  }
  auto& factory = sender_->GetValueFactory();
  if (factory == nullptr) {
    return;
  }
  auto entry_map = factory->CreateMap();
  entry_map->PushStringToMap("entryType", "memory");
  entry_map->PushStringToMap("name", "memory");
  float sizeKb = 0.f;
  if (memory_records_.empty()) {
    entry_map->PushDoubleToMap("sizeKb", sizeKb);
    sender_->OnPerformanceEvent(std::move(entry_map), kEventTypePlatform);
    return;
  }
  auto detail = sender_->GetValueFactory()->CreateMap();
  for (const auto& [category, record] : memory_records_) {
    auto record_map = sender_->GetValueFactory()->CreateMap();
    record_map->PushStringToMap("category", record.category_);
    record_map->PushDoubleToMap("sizeKb", record.size_kb_);
    sizeKb += record.size_kb_;
    if (record.detail_) {
      auto map = sender_->GetValueFactory()->CreateMap();
      for (const auto& [key, value] : *(record.detail_)) {
        map->PushStringToMap(key, value);
      }
      record_map->PushValueToMap("detail", std::move(map));
    }
    detail->PushValueToMap(category, std::move(record_map));
  }
  entry_map->PushDoubleToMap("sizeKb", sizeKb);
  entry_map->PushValueToMap("detail", std::move(detail));
  sender_->OnPerformanceEvent(std::move(entry_map), kEventTypePlatform);
}

}  // namespace performance
}  // namespace tasm
}  // namespace lynx
