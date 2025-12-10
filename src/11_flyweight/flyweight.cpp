//
// Created by Will George on 12/8/25.
//

#include "flyweight.h"

#include <iostream>
#include <utility>

Currency::Currency(const absl::string_view kCode, const uint8_t kDecimalPlaces,
                   const absl::string_view kSymbol)
    : code_(static_cast<std::string>(kCode)),
      decimal_places_(kDecimalPlaces),
      symbol_(static_cast<std::string>(kSymbol)) {}

absl::string_view Currency::GetCode() const { return code_; }

uint8_t Currency::GetDecimalPlaces() const { return decimal_places_; }

absl::string_view Currency::GetSymbol() const { return symbol_; }

absl::StatusOr<std::shared_ptr<ICurrency>> CurrencyFactory::GetCurrency(
    const absl::string_view kCode, const uint8_t kDecimalPlaces,
    const absl::string_view kSymbol) {
  auto ptr = std::make_shared<Currency>(static_cast<std::string>(kCode),
                                        kDecimalPlaces,
                                        static_cast<std::string>(kSymbol));
  auto [iter, inserted] =
      cache_.try_emplace(static_cast<std::string>(kCode), std::move(ptr));

  if (inserted) {
    std::cout << "New currency created: " << kCode << '\n';
  }
  return iter->second;
}

absl::StatusOr<std::shared_ptr<ICurrency>> CurrencyFactory::GetCurrency(
    const absl::string_view kCode) const {
  const auto kResult = cache_.find(static_cast<std::string>(kCode));
  if (kResult == cache_.end()) {
    return absl::NotFoundError("Currency not found");
  }

  return kResult->second;
}

size_t CurrencyFactory::GetCachedCurrencyCount() const { return cache_.size(); }

Payment::Payment(
    std::shared_ptr<ICurrency> cur,
    const std::int64_t kAmt,  // NOLINT(bugprone-easily-swappable-parameters)
    const std::uint64_t kTs)
    : currency_(std::move(cur)), amount_(kAmt), timestamp_(kTs) {}
