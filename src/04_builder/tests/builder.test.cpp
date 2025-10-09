//
// Created by Will George on 10/7/25.
//

#include "builder.h"

#include <algorithm>

#include <gtest/gtest.h>

class BuilderSuite : public ::testing::Test {
 protected:
  static constexpr std::string kTicker = "JPM";
  static constexpr std::uint64_t kExpDate = 1759795200;
};

class OptionSuite : public BuilderSuite {
 protected:
  static constexpr auto kRight = Right::kCall;
  static constexpr double kStrike = 150.0;
  static constexpr auto kSide = Side::kLong;
  static constexpr double kPremium = 10.0;
};

TEST_F(OptionSuite, ShouldConstructBasicOption) {
  const Option kOpt(kTicker, kExpDate, kRight, kStrike, kSide, kPremium);
  EXPECT_EQ(kOpt.ticker_, kTicker);
  EXPECT_EQ(kOpt.exp_date_, kExpDate);
  EXPECT_EQ(kOpt.right_, kRight);
  EXPECT_EQ(kOpt.strike_, kStrike);
  EXPECT_EQ(kOpt.side_, kSide);
  EXPECT_EQ(kOpt.premium_, kPremium);
}

class StrategySuite : public BuilderSuite {
 protected:
  static constexpr auto kRight = Right::kCall;

  static constexpr double kLongStrike = 150.0;
  static constexpr double kLongPremium = 10.0;

  static constexpr double kShortStrike = 160.0;
  static constexpr double kShortPremium = 5.0;

  static constexpr std::string kStrategy = "Bull Call";
  static constexpr std::uint8_t kBullCallLegCount = 2;

  inline static const Option kShortOption{
      kTicker, kExpDate, kRight, kShortStrike, Side::kShort, kShortPremium};

  inline static const Option kLongOption{
      kTicker, kExpDate, kRight, kLongStrike, Side::kLong, kLongPremium};
};

TEST_F(StrategySuite, ShouldConstructDefaultStategy) {
  const Strategy kStrategy{};
  EXPECT_EQ(kStrategy.name_, "");
  EXPECT_TRUE(kStrategy.legs_.empty());
}

TEST_F(StrategySuite, ShouldConstructBasicStrategy) {
  const Strategy kBasicStrategy(kStrategy);
  EXPECT_EQ(kBasicStrategy.name_, kStrategy);
  EXPECT_TRUE(kBasicStrategy.legs_.empty());
}

class SimpleStrategyBuilderSuite : public StrategySuite {};

TEST_F(SimpleStrategyBuilderSuite, ShouldBuildABullCallSpread) {
  SimpleStrategyBuilder builder;

  const Strategy kBullCall = builder.Reset(kStrategy)
                                 .Header(kTicker, kExpDate)
                                 .LongCall(kLongStrike, kLongPremium)
                                 .ShortCall(kShortStrike, kShortPremium)
                                 .Build();

  EXPECT_EQ(kBullCall.name_, kStrategy);
  EXPECT_EQ(kBullCall.legs_.size(), kBullCallLegCount);

  EXPECT_EQ(kBullCall.legs_[0], kLongOption);
  EXPECT_EQ(kBullCall.legs_[1], kShortOption);
}

class DirectorSuite : public StrategySuite {
 protected:
  static constexpr std::string kExpDateStr = "2025-10-07";

  static constexpr double kStrikePBuy = 200.0;
  static constexpr double kPremiumPBuy = 1.50;
  inline static const Option kLongPutOption{
      kTicker, kExpDate, Right::kPut, kStrikePBuy, Side::kLong, kPremiumPBuy};

  static constexpr double kStrikePSell = 210.0;
  static constexpr double kPremiumPSell = 3.25;
  inline static const Option kShortPutOption{kTicker,      kExpDate,
                                             Right::kPut,  kStrikePSell,
                                             Side::kShort, kPremiumPSell};

  static constexpr double kStrikeCSell = 240.0;
  static constexpr double kPremiumCSell = 3.50;
  inline static const Option kShortCallOption{kTicker,      kExpDate,
                                              Right::kCall, kStrikeCSell,
                                              Side::kShort, kPremiumCSell};

  static constexpr double kStrikeCBuy = 250.0;
  static constexpr double kPremiumCBuy = 1.75;
  inline static const Option kLongCallOption{
      kTicker, kExpDate, Right::kCall, kStrikeCBuy, Side::kLong, kPremiumCBuy};
};

TEST_F(DirectorSuite, ShouldBuildAnIronCondor) {
  SimpleStrategyBuilder builder;
  const StrategyDirector kStrategyDirector{builder};

  const Strategy kIronCondor = kStrategyDirector.IronCondor(
      kTicker, kExpDateStr, kStrikePBuy, kPremiumPBuy, kStrikePSell,
      kPremiumPSell, kStrikeCSell, kPremiumCSell, kStrikeCBuy, kPremiumCBuy);

  constexpr std::string kIronCondorName = "Iron Condor";
  constexpr uint8_t kIronCondorLegCount = 4;

  EXPECT_EQ(kIronCondor.name_, kIronCondorName);
  EXPECT_EQ(kIronCondor.legs_.size(), kIronCondorLegCount);

  const auto kEnd = kIronCondor.legs_.end();

  EXPECT_NE(std::ranges::find(kIronCondor.legs_, kLongPutOption), kEnd);
  EXPECT_NE(std::ranges::find(kIronCondor.legs_, kShortPutOption), kEnd);
  EXPECT_NE(std::ranges::find(kIronCondor.legs_, kShortCallOption), kEnd);
  EXPECT_NE(std::ranges::find(kIronCondor.legs_, kLongCallOption), kEnd);
}