//
// Created by Will George on 10/21/25.
//

#ifndef GOF23_BRIDGE_H
#define GOF23_BRIDGE_H

#include <memory>

// NOLINTBEGIN(readability-identifier-naming)

enum class Right : uint8_t { kCall, kPut };

struct PriceEngine {
  virtual ~PriceEngine() = default;
  [[nodiscard]] virtual double Price(double kSpot, double kStrike, double kTime,
                                     double kRate, double kSigma,
                                     Right kRight) const = 0;
};

struct BlackScholesEngine final : PriceEngine {
  static double NormalCDF(double kVal);

  [[nodiscard]] double Price(double kSpot, double kStrike, double kTime,
                             double kRate, double kSigma,
                             Right kRight) const override;
};

struct DummyEngine final : PriceEngine {
  [[nodiscard]] double Price(double kSpot, double kStrike, double kTime,
                             double kRate, double kSigma,
                             Right kRight) const override;
};

struct Option {
  virtual ~Option() = default;
  void SetEngine(std::shared_ptr<PriceEngine> engine);
  [[nodiscard]] virtual double Value() const = 0;

 protected:
  std::shared_ptr<PriceEngine> engine_;
};

struct EuropeanOption final : Option {
  explicit EuropeanOption(double kSpot, double kStrike, double kTime,
                          double kRate, double kSigma, Right kRight);
  [[nodiscard]] double Value() const override;

  double spot_, strike_, time_, rate_, sigma_;
  Right right_;
};
// NOLINTEND(readability-identifier-naming)

#endif  // GOF23_BRIDGE_H
