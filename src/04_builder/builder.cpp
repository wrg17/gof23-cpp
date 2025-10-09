//
// Created by Will George on 10/4/25.
//

#include "builder.h"

#include <iostream>
#include <utility>

Option::Option(std::string ticker, const std::uint64_t kExpDate,
               const Right kRight, const double kStrike, const Side kSide,
               const double kPremium)
    : ticker_(std::move(ticker)),
      exp_date_(kExpDate),
      right_(kRight),
      strike_(kStrike),
      side_(kSide),
      premium_(kPremium) {}

bool Option::operator==(const Option& rhs) const {
  return ticker_ == rhs.ticker_ && exp_date_ == rhs.exp_date_ &&
         right_ == rhs.right_ && strike_ == rhs.strike_ && side_ == rhs.side_ &&
         premium_ == rhs.premium_;
}

Strategy::Strategy() = default;

Strategy::Strategy(std::string name) : name_(std::move(name)) {}

IStrategyBuilder& SimpleStrategyBuilder::Add(const Right kRight,
                                             const Side kSide,
                                             const double kStrike,
                                             const double kPremium) {
  strategy_.legs_.emplace_back(ticker_, exp_date_, kRight, kStrike, kSide,
                               kPremium);
  return *this;
}

IStrategyBuilder& SimpleStrategyBuilder::Reset(const std::string& name) {
  strategy_ = Strategy(name);
  return *this;
}

IStrategyBuilder& SimpleStrategyBuilder::Header(const std::string& ticker,
                                                const std::uint64_t kExpDate) {
  ticker_ = ticker;
  exp_date_ = kExpDate;
  return *this;
}

IStrategyBuilder& SimpleStrategyBuilder::LongCall(const double kStrike,
                                                  const double kPremium) {
  return Add(Right::kCall, Side::kLong, kStrike, kPremium);
}

IStrategyBuilder& SimpleStrategyBuilder::ShortCall(const double kStrike,
                                                   const double kPremium) {
  return Add(Right::kCall, Side::kShort, kStrike, kPremium);
}

IStrategyBuilder& SimpleStrategyBuilder::LongPut(const double kStrike,
                                                 const double kPremium) {
  return Add(Right::kPut, Side::kLong, kStrike, kPremium);
}

IStrategyBuilder& SimpleStrategyBuilder::ShortPut(const double kStrike,
                                                  const double kPremium) {
  return Add(Right::kPut, Side::kShort, kStrike, kPremium);
}

Strategy SimpleStrategyBuilder::Build() {
  return std::exchange(strategy_, Strategy{});
}

StrategyDirector::StrategyDirector(IStrategyBuilder& builder)
    : builder_(builder) {}

Strategy StrategyDirector::IronCondor(
    const std::string& ticker, const std::string& exp_date,
    const double kStrikePBuy, const double kPremiumPBuy,
    const double kStrikePSell, const double kPremiumPSell,
    const double kStrikeCSell, const double kPremiumCSell,
    const double kStrikeCBuy, const double kPremiumCBuy) const {
  const std::uint64_t kExpDateUnix = DateStringToUnixTimestamp(exp_date);

  return builder_.Reset("Iron Condor")
      .Header(ticker, kExpDateUnix)
      .LongPut(kStrikePBuy, kPremiumPBuy)
      .ShortPut(kStrikePSell, kPremiumPSell)
      .ShortCall(kStrikeCSell, kPremiumCSell)
      .LongCall(kStrikeCBuy, kPremiumCBuy)
      .Build();
}