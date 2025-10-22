//
// Created by Will George on 10/21/25.
//

#include "bridge.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <numbers>

double BlackScholesEngine::NormalCDF(const double kVal) {
  constexpr double kHalf = 0.5;
  return kHalf * std::erfc(-kVal / std::numbers::sqrt2);
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
double BlackScholesEngine::Price(const double kSpot, const double kStrike,
                                 const double kTime, const double kRate,
                                 const double kSigma,
                                 const Right kRight) const {
  if (kTime <= 0 || kSigma <= 0) {
    return kRight == Right::kCall ? std::max(0.0, kSpot - kStrike)
                                  : std::max(0.0, kStrike - kSpot);
  }

  const double kSqrtT = std::sqrt(kTime);

  const double kD1 =
      (std::log(kSpot / kStrike) + (kRate + 0.5 * kSigma * kSigma) * kTime) /
      (kSigma * kSqrtT);

  const double kD2 = kD1 - (kSigma * kSqrtT);

  const double kDisc = std::exp(-kRate * kTime);

  if (kRight == Right::kCall) {
    return (kSpot * NormalCDF(kD1)) - (kStrike * kDisc * NormalCDF(kD2));
  }
  return (kStrike * kDisc * NormalCDF(-kD2)) - (kSpot * NormalCDF(-kD1));
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
double DummyEngine::Price(const double kSpot, const double kStrike,
                          [[maybe_unused]] const double kTime,
                          [[maybe_unused]] const double kRate,
                          [[maybe_unused]] const double kSigma,
                          const Right kRight) const {
  return std::max(
      0.0, kRight == Right::kCall ? (kSpot - kStrike) : (kStrike - kSpot));
};

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
EuropeanOption::EuropeanOption(const double kSpot, const double kStrike,
                               const double kTime, const double kRate,
                               const double kSigma, const Right kRight)
    : spot_(kSpot),
      strike_(kStrike),
      time_(kTime),
      rate_(kRate),
      sigma_(kSigma),
      right_(kRight) {};

double EuropeanOption::Value() const {
  return engine_->Price(spot_, strike_, time_, rate_, sigma_, right_);
}

void Option::SetEngine(std::shared_ptr<PriceEngine> engine) {
  engine_ = std::move(engine);
}