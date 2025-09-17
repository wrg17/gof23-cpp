//
// Created by Will George on 9/15/25.
//

#include "singleton.h"

#include <atomic>
#include <barrier>
#include <regex>
#include <string>
#include <thread>
#include <type_traits>
#include <vector>

#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

TEST(SingletonTest, InstanceReturnsSameObject) {
  Singleton& first = Singleton::Instance();
  Singleton& second = Singleton::Instance();
  EXPECT_EQ(&first, &second);
}

TEST(SingletonTest, CopyIsDeleted) {
  static_assert(!std::is_copy_constructible_v<Singleton>,
                "Singleton must not be copy constructible");
}

TEST(SingletonTest, AssignIsDeleted) {
  static_assert(!std::is_copy_assignable_v<Singleton>,
                "Singleton must not be copy assignable");
}

TEST(SingletonTest, ToStringReflectsName) {
  auto& singular = Singleton::Instance();
  singular.SetName("Alice");

  const std::string kText = singular.ToString();
  EXPECT_EQ(kText, "Singleton\n\tname: Alice\n");
}

TEST(SingletonTest, CastStringReflectsName) {
  auto& singular = Singleton::Instance();
  singular.SetName("Alice");

  const auto kStrCast = static_cast<std::string>(singular);
  EXPECT_EQ(kStrCast, "Singleton\n\tname: Alice\n");
}

TEST(SingletonTest, StreamOperatorPrintsSameAsToString) {
  auto& singular = Singleton::Instance();
  singular.SetName("Bob");

  std::ostringstream oss;
  oss << singular;

  EXPECT_EQ(oss.str(), singular.ToString());
}

TEST(SingletonTest, ThreadSafetyWhenSettingName) {
  const auto& singular = Singleton::Instance();

  constexpr int kThreads = 8;
  constexpr int kItersPerThread = 2000;

  std::barrier sync_point(kThreads);
  std::atomic done{false};

  std::vector<std::jthread> workers;
  workers.reserve(kThreads);
  for (int th = 0; th < kThreads; ++th) {
    workers.emplace_back([th, &sync_point]() {
      sync_point.arrive_and_wait();
      for (int i = 0; i < kItersPerThread; ++i) {
        Singleton::Instance().SetName(std::format("T{}-{}", th, i));
      }
    });
  }

  std::jthread reader([&done]() {
    const std::regex kPattern(R"(Singleton\n\tname: .*\n)");
    while (!done.load(std::memory_order_relaxed)) {
      const std::string kTs = Singleton::Instance().ToString();
      EXPECT_TRUE(std::regex_match(kTs, kPattern));
      std::this_thread::yield();
    }
  });

  workers.clear();
  done.store(true, std::memory_order_relaxed);

  const std::string kFinalText = singular.ToString();
  EXPECT_THAT(kFinalText, ::testing::HasSubstr("Singleton\n\tname: "));
}