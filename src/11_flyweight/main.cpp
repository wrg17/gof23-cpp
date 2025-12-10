//
// Created by Will George on 12/8/25.
//

#include <iostream>
#include <vector>

#include "flyweight.h"

int main() {
  constexpr absl::string_view kUSD = "USD";
  constexpr absl::string_view kSymbolUSD = "$";

  constexpr absl::string_view kEUR = "EUR";
  constexpr absl::string_view kSymbolEUR = "€";

  constexpr absl::string_view kGBP = "GBP";
  constexpr absl::string_view kSymbolGBP = "£";

  constexpr uint8_t kStandardDecimals = 2;

  CurrencyFactory factory{};

  auto add_currency = [&factory](const absl::string_view kCode,
                                 const uint8_t kDecimalPlaces,
                                 const absl::string_view kSymbol) -> void {
    const auto kStatusOrCurrency =
        factory.GetCurrency(kCode, kDecimalPlaces, kSymbol);

    if (!kStatusOrCurrency.ok()) {
      std::cout << "Failed: " << kSymbol << '\n';
      return;
    }
    std::cout << "Succeeded: " << kSymbol << '\n';
  };

  add_currency(kUSD, kStandardDecimals, kSymbolUSD);
  add_currency(kEUR, kStandardDecimals, kSymbolEUR);
  add_currency(kGBP, kStandardDecimals, kSymbolGBP);

  std::vector<Payment> payments{};

  auto create_txn = [&factory, &payments](const absl::string_view kCode,
                                          const double kAmount) {
    const auto kStatusOrCurrency = factory.GetCurrency(kCode);
    if (!kStatusOrCurrency.ok()) {
      std::cout << "Failed to get currency: " << kCode << '\n';
      return;
    }

    std::shared_ptr<ICurrency> currency = std::move(kStatusOrCurrency).value();

    const auto kAmountInSmallestUnits =
        static_cast<std::int64_t>(kAmount * kPriceMultiplier);

    constexpr std::uint64_t kTimestamp = 1702000000;

    payments.emplace_back(currency, kAmountInSmallestUnits, kTimestamp);
  };

  constexpr double kPayment1Amount = 100.00;
  constexpr double kPayment2Amount = 200.00;
  constexpr double kPayment3Amount = 300.00;
  constexpr double kPayment4Amount = 400.00;
  constexpr double kPayment5Amount = 500.00;
  constexpr double kPayment6Amount = 600.00;

  create_txn(kUSD, kPayment1Amount);
  create_txn(kUSD, kPayment2Amount);
  create_txn(kUSD, kPayment3Amount);
  create_txn(kEUR, kPayment4Amount);
  create_txn(kEUR, kPayment5Amount);
  create_txn(kGBP, kPayment6Amount);

  std::cout << "\n=== Flyweight Pattern: Currency Sharing in Payments ===\n\n";

  std::cout << "Payments created:\n";
  for (size_t i = 0; i < payments.size(); ++i) {
    const auto& payment = payments[i];
    const double kDisplayAmount =
        static_cast<double>(payment.amount_) / kPriceMultiplier;
    std::cout << "  Payment " << (i + 1) << ": " << kDisplayAmount << " "
              << payment.currency_->GetSymbol() << payment.currency_->GetCode()
              << " (Currency ptr: " << payment.currency_.get() << ")\n";
  }

  std::cout << "\nFlyweight sharing verification:\n";
  std::cout << "  Total payments: " << payments.size() << "\n";
  std::cout << "  Cached currencies: " << factory.GetCachedCurrencyCount()
            << "\n";

  const bool kUsdPaymentsShare =
      (payments[0].currency_.get() == payments[1].currency_.get()) &&
      (payments[1].currency_.get() == payments[2].currency_.get());
  std::cout << "  USD payments share same Currency object: "
            << (kUsdPaymentsShare ? "YES" : "NO") << "\n";

  const bool kEurPaymentsShare =
      (payments[3].currency_.get() == payments[4].currency_.get());
  std::cout << "  EUR payments share same Currency object: "
            << (kEurPaymentsShare ? "YES" : "NO") << "\n";

  std::cout << "\nMemory efficiency demonstration:\n";
  std::cout << "  " << payments.size() << " payments created\n";
  std::cout << "  Only " << factory.GetCachedCurrencyCount()
            << " Currency flyweight objects in memory\n";
  std::cout << "  Memory savings: "
            << (payments.size() - factory.GetCachedCurrencyCount())
            << " fewer Currency objects due to sharing!\n";

  return 0;
}