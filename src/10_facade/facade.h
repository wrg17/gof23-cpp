//
// Created by Will George on 11/10/25.
//

#ifndef GOF23_FACADE_H
#define GOF23_FACADE_H

#include <absl/container/flat_hash_map.h>
#include <absl/status/statusor.h>

constexpr std::int64_t kPriceMultiplier = 10000;

using SymbolId = std::uint32_t;

struct Order {
  SymbolId symbol_{};
  std::int64_t price_{};
  std::int64_t quantity_{};
};

struct SymbolTable {
  absl::flat_hash_map<std::string, SymbolId> to_id_;
  std::vector<std::string> to_text_;

  std::uint32_t Add(
      absl::string_view kSymbolText  // NOLINT(readability-identifier-naming)
  );
  [[nodiscard]] absl::string_view Text(
      SymbolId kSymbolId  // NOLINT(readability-identifier-naming)
  ) const;
};

class PricingService {
 public:
  explicit PricingService(
      absl::flat_hash_map<SymbolId, std::int64_t>&& price_by_id);

  [[nodiscard]] absl::StatusOr<std::int64_t> GetFairPrice(
      SymbolId kSymbol  // NOLINT(readability-identifier-naming)
  ) const;

 private:
  absl::flat_hash_map<SymbolId, std::int64_t> price_by_id_;
};

class RiskService {
 public:
  explicit RiskService(
      std::int64_t kNotionalLimit  // NOLINT(readability-identifier-naming)
  );

  [[nodiscard]] absl::Status WithinLimit(const Order& order) const;

 private:
  std::int64_t notional_limit_;
};

class ExecutionService {
 public:
  explicit ExecutionService(SymbolTable&& symbol_table);

  absl::Status SendOrder(const Order& order);

 private:
  SymbolTable symbol_table_;
  std::uint64_t order_id_{0};
};

class TradingFacade {
 public:
  explicit TradingFacade(PricingService&& pricing, RiskService&& risk,
                         ExecutionService&& exec);

  absl::Status PlaceMarketOrder(
      SymbolId kSymbol,       // NOLINT(readability-identifier-naming)
      std::int64_t kQuantity  // NOLINT(readability-identifier-naming)
  );

 private:
  PricingService pricing_;
  RiskService risk_;
  ExecutionService exec_;
};

#endif  // GOF23_FACADE_H
