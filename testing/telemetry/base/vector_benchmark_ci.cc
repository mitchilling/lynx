// Copyright 2025 The Lynx Authors. All rights reserved.
// Licensed under the Apache License Version 2.0 that can be found in the
// LICENSE file in the root directory of this source tree.

#include <algorithm>
#include <memory>
#include <random>
#include <string>

#include "base/include/value/base_value.h"
#include "base/include/value/table.h"
#include "base/include/vector.h"
#include "third_party/benchmark/include/benchmark/benchmark.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"

namespace lynx {
namespace base {

static void BM_VectorStringPushBack(benchmark::State& state) {
  Vector<String> source_strings;
  for (int i = 0; i < 1000; i++) {
    source_strings.emplace_back(std::string("key_") + std::to_string(i));
  }

  for (auto _ : state) {
    Vector<String> array;
    array.reserve(source_strings.size() * 5);
    state.ResumeTiming();
    for (int i = 0; i < 5; i++) {
      for (const auto& s : source_strings) {
        array.push_back(s);
      }
    }
    state.PauseTiming();  // Ignores array destruction time
  }
}

static void BM_VectorStringPushBackNoReserve(benchmark::State& state) {
  Vector<String> source_strings;
  for (int i = 0; i < 1000; i++) {
    source_strings.emplace_back(std::string("key_") + std::to_string(i));
  }

  for (auto _ : state) {
    Vector<String> array;
    state.ResumeTiming();
    for (int i = 0; i < 5; i++) {
      for (const auto& s : source_strings) {
        array.push_back(s);
      }
    }
    state.PauseTiming();  // Ignores array destruction time
  }
}

static void BM_VectorStringPushBackNoReserveSmall(benchmark::State& state) {
  Vector<String> source_strings;
  for (int i = 0; i < 20; i++) {
    source_strings.emplace_back(std::string("key_") + std::to_string(i));
  }

  for (auto _ : state) {
    Vector<Vector<String>> arrays;
    for (int i = 0; i < 1000; i++) {
      arrays.emplace_back();
    }
    state.ResumeTiming();
    for (auto& array : arrays) {
      for (const auto& s : source_strings) {
        array.push_back(s);
      }
    }
    state.PauseTiming();  // Ignores array destruction time
  }
}

static void BM_VectorStringInsertSmall(benchmark::State& state) {
  Vector<String> source_strings;
  for (int i = 0; i < 20; i++) {
    source_strings.emplace_back(std::string("key_") + std::to_string(i));
  }

  for (auto _ : state) {
    Vector<Vector<String>> arrays;
    for (int i = 0; i < 1000; i++) {
      arrays.emplace_back();
    }
    state.ResumeTiming();
    for (auto& array : arrays) {
      for (const auto& s : source_strings) {
        array.insert(array.begin(), s);
      }
    }
    state.PauseTiming();  // Ignores array destruction time
  }
}

static void BM_VectorStringEraseSmall(benchmark::State& state) {
  Vector<String> source_strings;
  for (int i = 0; i < 20; i++) {
    source_strings.emplace_back(std::string("key_") + std::to_string(i));
  }

  for (auto _ : state) {
    Vector<Vector<String>> arrays;
    for (int i = 0; i < 1000; i++) {
      auto& array = arrays.emplace_back();
      for (const auto& s : source_strings) {
        array.push_back(s);
      }
    }
    state.ResumeTiming();
    for (auto& array : arrays) {
      while (array.size() > 0) {
        array.erase(array.begin());
      }
    }
    state.PauseTiming();  // Ignores array destruction time
  }
}

BENCHMARK(BM_VectorStringPushBack);
BENCHMARK(BM_VectorStringPushBackNoReserve);
BENCHMARK(BM_VectorStringPushBackNoReserveSmall);
BENCHMARK(BM_VectorStringInsertSmall);
BENCHMARK(BM_VectorStringEraseSmall);

#define STRINGIFY(s) #s
#define FOREACH_STRINGIFY_1(a) #a
#define FOREACH_STRINGIFY_2(a, ...) #a ", " FOREACH_STRINGIFY_1(__VA_ARGS__)
#define FOREACH_STRINGIFY_3(a, ...) #a ", " FOREACH_STRINGIFY_2(__VA_ARGS__)
#define FOREACH_STRINGIFY_4(a, ...) #a ", " FOREACH_STRINGIFY_3(__VA_ARGS__)
#define FOREACH_STRINGIFY_N(_4, _3, _2, _1, N, ...) FOREACH_STRINGIFY##N
#define FOREACH_STRINGIFY(...)                     \
  FOREACH_STRINGIFY_N(__VA_ARGS__, _4, _3, _2, _1) \
  (__VA_ARGS__)

using namespace std;          // NOLINT
using namespace lynx::lepus;  // NOLINT

namespace {
template <class T>
T Cast(size_t v);

template <>
String Cast(size_t v) {
  if (v % 2 == 0) {
    return String(std::string("string_") + to_string(v));
  } else {
    return String(to_string(v) + std::string("_string"));
  }
}

template <>
Value Cast(size_t v) {
  if (v % 3 == 1) {
    return Value(std::to_string(v));
  } else if (v % 3 == 2) {
    return Value(lepus::Dictionary::Create());
  } else {
    return Value((int32_t)v);
  }
}
}  // namespace

#define TEST_FUNC_SET_INSERT(TYPE, DATA_COUNT, SET, ...)        \
  static void BM_##SET##_insert_##DATA_COUNT##_##TYPE(          \
      benchmark::State& state) {                                \
    using SetType = SET<__VA_ARGS__>;                           \
    constexpr size_t kDataCount = DATA_COUNT;                   \
    constexpr size_t kDestructionBatchCount = 50;               \
    vector<SetType::key_type> data;                             \
                                                                \
    /* Generate data. Keys should not be ordered. */            \
    int data_index = 0;                                         \
    data.resize(kDataCount);                                    \
    for (size_t i = 0; i < kDataCount / 2; i++) {               \
      data[data_index++] = Cast<SetType::key_type>(i);          \
    }                                                           \
    for (size_t i = kDataCount - 1; i >= kDataCount / 2; i--) { \
      data[data_index++] = Cast<SetType::key_type>(i);          \
    }                                                           \
                                                                \
    size_t total = 0;                                           \
    for (auto _ : state) {                                      \
      SetType* sets[kDestructionBatchCount];                    \
      for (size_t i = 0; i < kDestructionBatchCount; i++) {     \
        sets[i] = new SetType();                                \
        for (const auto& key : data) {                          \
          sets[i]->insert(key);                                 \
        }                                                       \
        total += sets[i]->size();                               \
      }                                                         \
      state.PauseTiming();                                      \
      for (size_t i = 0; i < kDestructionBatchCount; i++) {     \
        delete sets[i]; /* destruction time not measured */     \
      }                                                         \
      state.ResumeTiming();                                     \
    }                                                           \
  }

#define TEST_FUNC_MAP_INSERT(TYPE, DATA_COUNT, MAP, ...)         \
  static void BM_##MAP##_insert_##DATA_COUNT##_##TYPE(           \
      benchmark::State& state) {                                 \
    using MapType = MAP<__VA_ARGS__>;                            \
    constexpr size_t kDataCount = DATA_COUNT;                    \
    constexpr size_t kDestructionBatchCount = 50;                \
    vector<pair<MapType::key_type, MapType::mapped_type>> data;  \
                                                                 \
    /* Generate data. Keys should not be ordered. */             \
    int data_index = 0;                                          \
    data.resize(kDataCount);                                     \
    for (size_t i = 0; i < kDataCount / 2; i++) {                \
      data[data_index].first = Cast<MapType::key_type>(i);       \
      data[data_index++].second = Cast<MapType::mapped_type>(i); \
    }                                                            \
    for (size_t i = kDataCount - 1; i >= kDataCount / 2; i--) {  \
      data[data_index].first = Cast<MapType::key_type>(i);       \
      data[data_index++].second = Cast<MapType::mapped_type>(i); \
    }                                                            \
                                                                 \
    size_t total = 0;                                            \
    for (auto _ : state) {                                       \
      MapType* maps[kDestructionBatchCount];                     \
      for (size_t i = 0; i < kDestructionBatchCount; i++) {      \
        maps[i] = new MapType();                                 \
        for (auto it = data.begin(); it != data.end(); it++) {   \
          (*maps[i])[it->first] = it->second;                    \
        }                                                        \
        total += maps[i]->size();                                \
      }                                                          \
      state.PauseTiming();                                       \
      for (size_t i = 0; i < kDestructionBatchCount; i++) {      \
        delete maps[i]; /* destruction time not measured */      \
      }                                                          \
      state.ResumeTiming();                                      \
    }                                                            \
  }

#define TEST_SET_INSERT_S(DATA_COUNT, SET, ...)         \
  TEST_FUNC_SET_INSERT(S, DATA_COUNT, SET, __VA_ARGS__) \
  BENCHMARK(BM_##SET##_insert_##DATA_COUNT##_S)

#define TEST_MAP_INSERT_SV(DATA_COUNT, MAP, ...)         \
  TEST_FUNC_MAP_INSERT(SV, DATA_COUNT, MAP, __VA_ARGS__) \
  BENCHMARK(BM_##MAP##_insert_##DATA_COUNT##_SV)

TEST_SET_INSERT_S(32, OrderedFlatSet, String);
TEST_SET_INSERT_S(32, LinearFlatSet, String);

TEST_MAP_INSERT_SV(32, OrderedFlatMap, String, Value);
TEST_MAP_INSERT_SV(32, LinearFlatMap, String, Value);

}  // namespace base
}  // namespace lynx

#pragma clang diagnostic pop
