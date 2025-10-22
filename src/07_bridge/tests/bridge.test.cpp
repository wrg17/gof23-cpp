//
// Created by Will George on 10/21/25.
//

#include "bridge.h"

#include <gtest/gtest.h>

class BridgeSuite : public ::testing::Test {
 protected:
  static constexpr double kSpot = 120;
  static constexpr double kStrike = 100;
  static constexpr double kTime = 1;
  static constexpr double kRate = 0.01;
  static constexpr double kSigma = 0.25;
  static constexpr auto kRight = Right::kCall;
};

class BlackScholesEngineSuite : public BridgeSuite {
 protected:
  static constexpr BlackScholesEngine kBS{};
};

TEST_F(BlackScholesEngineSuite, ShouldGetTheNormalCdf) {
  const double kMiddle = BlackScholesEngine::NormalCDF(0);
  EXPECT_EQ(kMiddle, 0.5);
}

TEST_F(BlackScholesEngineSuite, ShouldGetTheCallPrice) {
  const double kPrice = kBS.Price(kSpot, kStrike, kTime, kRate, kSigma, kRight);
  EXPECT_EQ(kPrice, 24.435982086604881);
}

TEST_F(BlackScholesEngineSuite, ShouldGetThePutPrice) {
  const double kPrice =
      kBS.Price(kSpot, kStrike, kTime, kRate, kSigma, Right::kPut);
  EXPECT_EQ(kPrice, 3.4409654615216922);
}

TEST_F(BlackScholesEngineSuite, ShouldHandleZeroSpot) {
  const double kPrice = kBS.Price(0, kStrike, kTime, kRate, kSigma, kRight);
  EXPECT_EQ(kPrice, 0.0);
}

TEST_F(BlackScholesEngineSuite, ShouldHandleZeroStrike) {
  const double kPrice = kBS.Price(kSpot, 0, kTime, kRate, kSigma, kRight);
  EXPECT_EQ(kPrice, kSpot);
}

TEST_F(BlackScholesEngineSuite, ShouldHandleZeroTime) {
  const double kPrice = kBS.Price(kSpot, kStrike, 0, kRate, kSigma, kRight);
  EXPECT_EQ(kPrice, std::max(kSpot - kStrike, 0.0));
}

TEST_F(BlackScholesEngineSuite, ShouldHandleZeroVolatility) {
  const double kPrice = kBS.Price(kSpot, kStrike, kTime, kRate, 0, kRight);
  EXPECT_EQ(kPrice, std::max(kSpot - kStrike, 0.0));
}

class DummyEngineSuite : public BridgeSuite {
 protected:
  static constexpr DummyEngine kDummy{};
};

TEST_F(DummyEngineSuite, ShouldGetThePrice) {
  const double kPrice =
      kDummy.Price(kSpot, kStrike, kTime, kRate, kSigma, kRight);
  EXPECT_EQ(kPrice, 20);
}

class OptionSuite : public BridgeSuite {};

TEST_F(OptionSuite, ShouldUpdateTheEngine) {
  auto option = EuropeanOption(kSpot, kStrike, kTime, kRate, kSigma, kRight);
  option.SetEngine(std::make_shared<BlackScholesEngine>());
  const auto kBsPrice = option.Value();

  option.SetEngine(std::make_shared<DummyEngine>());
  const auto kDummyPrice = option.Value();
  EXPECT_NE(kBsPrice, kDummyPrice);
}