/*
 *
 * Copyright (C) 2020 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "gtest/gtest.h"

#include "logging/logging.hpp"
#include "utils/utils.hpp"
#include "test_harness/test_harness.hpp"

namespace lzt = level_zero_tests;

#include <level_zero/zes_api.h>

namespace {

class EngineModuleTest : public lzt::SysmanCtsClass {};

TEST_F(
    EngineModuleTest,
    GivenComponentCountZeroWhenRetrievingSysmanEngineHandlesThenNonZeroCountIsReturned) {
  for (auto device : devices) {
    lzt::get_engine_handle_count(device);
  }
}

TEST_F(
    EngineModuleTest,
    GivenComponentCountZeroWhenRetrievingSysmanEngineHandlesThenNotNullEngineHandlesAreReturned) {
  for (auto device : devices) {
    uint32_t count = 0;
    auto engineHandles = lzt::get_engine_handles(device, count);
    ASSERT_EQ(engineHandles.size(), count);
    for (auto engineHandle : engineHandles) {
      EXPECT_NE(nullptr, engineHandle);
    }
  }
}

TEST_F(
    EngineModuleTest,
    GivenInvalidComponentCountWhenRetrievingSysmanEngineHandlesThenActualComponentCountIsUpdated) {
  for (auto device : devices) {
    uint32_t actualCount = 0;
    lzt::get_engine_handles(device, actualCount);
    uint32_t testCount = actualCount + 1;
    lzt::get_engine_handles(device, testCount);
    EXPECT_EQ(testCount, actualCount);
  }
}

TEST_F(
    EngineModuleTest,
    GivenValidComponentCountWhenCallingApiTwiceThenSimilarEngineHandlesReturned) {
  for (auto device : devices) {
    uint32_t count = 0;
    auto engineHandlesInitial = lzt::get_engine_handles(device, count);
    for (auto engineHandle : engineHandlesInitial) {
      EXPECT_NE(nullptr, engineHandle);
    }

    count = 0;
    auto engineHandlesLater = lzt::get_engine_handles(device, count);
    for (auto engineHandle : engineHandlesLater) {
      EXPECT_NE(nullptr, engineHandle);
    }
    EXPECT_EQ(engineHandlesInitial, engineHandlesLater);
  }
}

TEST_F(
    EngineModuleTest,
    GivenValidEngineHandleWhenRetrievingEnginePropertiesThenValidPropertiesAreReturned) {
  for (auto device : devices) {
    uint32_t count = 0;
    auto engineHandles = lzt::get_engine_handles(device, count);
    for (auto engineHandle : engineHandles) {
      ASSERT_NE(nullptr, engineHandle);
      auto properties = lzt::get_engine_properties(engineHandle);
      EXPECT_GE(properties.type, ZES_ENGINE_GROUP_ALL);
      EXPECT_LE(properties.type, ZES_ENGINE_GROUP_COPY_SINGLE);
      if (properties.onSubdevice == true) {
        EXPECT_LT(properties.subdeviceId, UINT32_MAX);
      } else {
        EXPECT_GE(properties.subdeviceId, 0);
      }
    }
  }
}

TEST_F(
    EngineModuleTest,
    GivenValidEngineHandleWhenRetrievingEnginePropertiesThenExpectSamePropertiesReturnedTwice) {
  for (auto device : devices) {
    uint32_t count = 0;
    auto engineHandles = lzt::get_engine_handles(device, count);
    for (auto engineHandle : engineHandles) {
      EXPECT_NE(nullptr, engineHandle);
      auto propertiesInitial = lzt::get_engine_properties(engineHandle);
      auto propertiesLater = lzt::get_engine_properties(engineHandle);
      EXPECT_EQ(propertiesInitial.type, propertiesLater.type);
      EXPECT_EQ(propertiesInitial.onSubdevice, propertiesLater.onSubdevice);
      if (propertiesInitial.onSubdevice == true &&
          propertiesLater.onSubdevice == true) {
        EXPECT_EQ(propertiesInitial.subdeviceId, propertiesLater.subdeviceId);
      }
    }
  }
}

TEST_F(
    EngineModuleTest,
    GivenValidEngineHandleWhenRetrievingEngineActivityStatsThenValidStatsIsReturned) {
  for (auto device : devices) {
    uint32_t count = 0;
    auto engineHandles = lzt::get_engine_handles(device, count);
    for (auto engineHandle : engineHandles) {
      ASSERT_NE(nullptr, engineHandle);
      auto state = lzt::get_engine_activity(engineHandle);
      EXPECT_LT(state.activeTime, UINT32_MAX);
      EXPECT_LT(state.timestamp, UINT32_MAX);
    }
  }
}
TEST_F(
    EngineModuleTest,
    GivenValidEngineHandleWhenRetrievingEngineActivityStatsThenTimestampWillbeIncrementedInNextCalltoEngineActivity) {
  for (auto device : devices) {
    uint32_t count = 0;
    auto engineHandles = lzt::get_engine_handles(device, count);
    for (auto engineHandle : engineHandles) {
      ASSERT_NE(nullptr, engineHandle);
      auto oldstate = lzt::get_engine_activity(engineHandle);
      EXPECT_LT(oldstate.activeTime, UINT32_MAX);
      EXPECT_LT(oldstate.timestamp, UINT32_MAX);
      auto newstate = lzt::get_engine_activity(engineHandle);
      EXPECT_LT(newstate.activeTime, UINT32_MAX);
      EXPECT_LT(newstate.timestamp, UINT32_MAX);
      EXPECT_GT(newstate.timestamp, oldstate.timestamp);
    }
  }
}
} // namespace
