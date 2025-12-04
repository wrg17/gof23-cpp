//
// Created by Will George on 11/10/25.
//

#include <iostream>

#include "facade.h"

int main() {
  SymbolTable symbols;
  const SymbolId kAapl = symbols.Add("AAPL");

  absl::flat_hash_map<SymbolId, std::int64_t> price_by_id;
  price_by_id.emplace(kAapl,
                      static_cast<std::int64_t>(190.25 * kPriceMultiplier));

  constexpr auto notionalLimit =
      static_cast<std::int64_t>(2'000'000.0 * kPriceMultiplier);

  PricingService pricing(std::move(price_by_id));
  RiskService risk(notionalLimit);
  ExecutionService exec(std::move(symbols));

  TradingFacade facade(std::move(pricing), std::move(risk), std::move(exec));

  constexpr std::int64_t qty = 10'000;
  const absl::Status kOrderStatus = facade.PlaceMarketOrder(kAapl, qty);
  if (!kOrderStatus.ok()) std::cout << kOrderStatus << "\n";
  return kOrderStatus.ok() ? 0 : 1;
}