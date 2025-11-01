//
// Created by Will George on 10/24/25.
//

#ifndef GOF23_COMPOSITE_H
#define GOF23_COMPOSITE_H

#include <memory>
#include <string>

#include <absl/status/status.h>
#include <absl/status/statusor.h>

enum class Right : uint8_t { kCall, kPut };

struct Ticker {
  explicit Ticker(std::string symbol, std::string name);

  [[nodiscard]] const std::string& Symbol() const;

  [[nodiscard]] const std::string& Name() const;

 private:
  std::string symbol_;
  std::string name_;
};

struct RiskNode {
  virtual ~RiskNode() = default;
  [[nodiscard]] virtual double NetCost() const = 0;
  virtual absl::Status Add(std::unique_ptr<RiskNode>);
  [[nodiscard]] virtual std::string ToString() const = 0;
};

class Stock final : public RiskNode {
 public:
  static absl::StatusOr<std::unique_ptr<Stock>> Create(
      Ticker ticker,
      uint32_t kQuantity,  // NOLINT(readability-identifier-naming)
      double kPrice);      // NOLINT(readability-identifier-naming)

  [[nodiscard]] double NetCost() const override;

  [[nodiscard]] std::string ToString() const override;

 private:
  Stock(Ticker ticker,
        uint32_t kQuantity,  // NOLINT(readability-identifier-naming)
        double kPrice);      // NOLINT(readability-identifier-naming)

  Ticker ticker_;
  uint32_t quantity_;
  double price_;
};

class Portfolio final : public RiskNode {
 public:
  static absl::StatusOr<std::unique_ptr<Portfolio>> Create();

  absl::Status Add(std::unique_ptr<RiskNode> node) override;

  [[nodiscard]] double NetCost() const override;

  [[nodiscard]] const std::vector<std::unique_ptr<RiskNode>>& Children() const;

  [[nodiscard]] std::string ToString() const override;

 private:
  explicit Portfolio();

  std::vector<std::unique_ptr<RiskNode>> children_;
};

#endif  // GOF23_COMPOSITE_H
