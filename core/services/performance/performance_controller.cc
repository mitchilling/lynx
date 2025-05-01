// Copyright 2025 The Lynx Authors. All rights reserved.
// Licensed under the Apache License Version 2.0 that can be found in the
// LICENSE file in the root directory of this source tree.

#include "core/services/performance/performance_controller.h"

#include <utility>

namespace lynx {
namespace tasm {
namespace performance {

PerformanceController::PerformanceController(PerformanceController&& other)
    : instance_id_(other.instance_id_),
      value_factory_(std::move(other.value_factory_)),
      memory_monitor_(std::move(other.memory_monitor_)) {
  memory_monitor_.delegate_ = this;
  // reset other
  other.instance_id_ = report::kUninitializedInstanceId;
  other.value_factory_ = nullptr;
}

PerformanceController& PerformanceController::operator=(
    PerformanceController&& other) {
  if (this != &other) {
    // move
    instance_id_ = other.instance_id_;
    value_factory_ = std::move(other.value_factory_);
    memory_monitor_ = std::move(other.memory_monitor_);
    // update delegate_
    memory_monitor_.delegate_ = this;
    // reset other
    other.instance_id_ = report::kUninitializedInstanceId;
    other.value_factory_ = nullptr;
  }
  return *this;
}
void PerformanceController::SetActor(
    const std::shared_ptr<shell::LynxActor<PerformanceController>>& actor) {
  // TODO: Implement Darwin & Android platform support later.
}

void PerformanceController::OnPerformanceEvent(
    const std::unique_ptr<pub::Value> entry) {
  // TODO: Implement Darwin & Android platform support later.
}

}  // namespace performance
}  // namespace tasm
}  // namespace lynx
