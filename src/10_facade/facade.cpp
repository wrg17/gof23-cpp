//
// Created by Will George on 11/10/25.
//

#include "facade.h"

#include <random>
#include <utility>

SymbolId SymbolTable::Add(const absl::string_view kSymbolText) {
  auto kDefaultId = static_cast<SymbolId>(to_text_.size());

  auto [iter, inserted] = to_id_.try_emplace(kSymbolText, kDefaultId);
  if (inserted) {
    to_text_.emplace_back(iter->first);
  }
  return iter->second;
}

[[nodiscard]] absl::string_view SymbolTable::Text(
    const SymbolId kSymbolId) const {
  return to_text_[kSymbolId];
}

PricingService::PricingService(
    absl::flat_hash_map<SymbolId, std::int64_t>&& price_by_id)
    : price_by_id_(std::move(price_by_id)) {}

absl::StatusOr<std::int64_t> PricingService::GetFairPrice(
    const SymbolId kSymbol) const {
  const auto kIter = price_by_id_.find(kSymbol);
  if (kIter == price_by_id_.end()) {
    return absl::NotFoundError("symbol not found");
  }

  const std::int64_t kPrice = kIter->second;
  std::cout << "[sym_id=" << kSymbol << "] priced @ "
            << (static_cast<double>(kPrice) /
                static_cast<double>(kPriceMultiplier))
            << "\n";
  return kPrice;
}

RiskService::RiskService(const std::int64_t kNotionalLimit)
    : notional_limit_(kNotionalLimit) {}

absl::Status RiskService::WithinLimit(const Order& order) const {
  if (const std::int64_t notional = order.price_ * order.quantity_;
      notional > notional_limit_) {
    std::cout << "Risk: notional " << notional << " > limit " << notional_limit_
              << "\n";
    return absl::ResourceExhaustedError("risk limit exceeded");
  }
  return absl::OkStatus();
}

ExecutionService::ExecutionService(SymbolTable&& symbol_table)
    : symbol_table_(std::move(symbol_table)) {}

absl::Status ExecutionService::SendOrder(const Order& order) {
  ++order_id_;

  const double kPrice =
      static_cast<double>(order.price_) / static_cast<double>(kPriceMultiplier);

  const absl::string_view kSym = symbol_table_.Text(order.symbol_);

  std::cout << std::format("EXECUTING BUY {} {} @ {} [id={}, sym_id={}]\n",
                           order.quantity_, kSym, kPrice, order_id_,
                           order.symbol_);
  return absl::OkStatus();
}

TradingFacade::TradingFacade(PricingService&& pricing, RiskService&& risk,
                             ExecutionService&& exec)
    : pricing_(std::move(pricing)),
      risk_(std::move(risk)),
      exec_(std::move(exec)) {}

absl::Status TradingFacade::PlaceMarketOrder(
    const SymbolId kSymbol,  // NOLINT(bugprone-easily-swappable-parameters)
    const std::int64_t kQuantity) {
  auto price_or = pricing_.GetFairPrice(kSymbol);
  if (!price_or.ok()) return price_or.status();

  const Order kOrder{kSymbol, *price_or, kQuantity};

  if (absl::Status status = risk_.WithinLimit(kOrder); !status.ok()) {
    return status;
  }

  return exec_.SendOrder(kOrder);
}