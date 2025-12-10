
//
// Created by Will George on 12/8/25.
//

#ifndef GOF23_FLYWEIGHT_H
#define GOF23_FLYWEIGHT_H

#include <memory>
#include <string>

#include <absl/container/flat_hash_map.h>
#include <absl/status/statusor.h>

constexpr std::int64_t kPriceMultiplier = 10000;

class ICurrency {
 public:
  virtual ~ICurrency() = default;
  [[nodiscard]] virtual absl::string_view GetCode() const = 0;
  [[nodiscard]] virtual uint8_t GetDecimalPlaces() const = 0;
  [[nodiscard]] virtual absl::string_view GetSymbol() const = 0;
};

class Currency final : public ICurrency {
 public:
  Currency(absl::string_view kCode,     // NOLINT(readability-identifier-naming)
           uint8_t kDecimalPlaces,      // NOLINT(readability-identifier-naming)
           absl::string_view kSymbol);  // NOLINT(readability-identifier-naming)

  [[nodiscard]] absl::string_view GetCode() const override;
  [[nodiscard]] uint8_t GetDecimalPlaces() const override;
  [[nodiscard]] absl::string_view GetSymbol() const override;

 private:
  std::string code_;
  uint8_t decimal_places_;
  std::string symbol_;
};

class CurrencyFactory {
 public:
  [[nodiscard]] absl::StatusOr<std::shared_ptr<ICurrency>> GetCurrency(
      absl::string_view kCode,     // NOLINT(readability-identifier-naming)
      uint8_t kDecimalPlaces,      // NOLINT(readability-identifier-naming)
      absl::string_view kSymbol);  // NOLINT(readability-identifier-naming)

  [[nodiscard]] absl::StatusOr<std::shared_ptr<ICurrency>> GetCurrency(
      absl::string_view kCode) const;  // NOLINT(readability-identifier-naming)

  [[nodiscard]] size_t GetCachedCurrencyCount() const;

 private:
  absl::flat_hash_map<std::string, std::shared_ptr<ICurrency>> cache_;
};

struct Payment {
  std::shared_ptr<ICurrency> currency_;
  std::int64_t amount_;
  std::uint64_t timestamp_;

  Payment(std::shared_ptr<ICurrency> cur,
          std::int64_t kAmt,   // NOLINT(readability-identifier-naming)
          std::uint64_t kTs);  // NOLINT(readability-identifier-naming)
};

#endif  // GOF23_FLYWEIGHT_H