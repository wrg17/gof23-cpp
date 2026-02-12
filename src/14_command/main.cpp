//
// Created by Will George on 1/8/26.
//

#include <iostream>

#include "command.h"

namespace {
constexpr std::uint64_t kOrderBookId = 54321;
constexpr std::uint64_t kPortfolioId = 12345;
constexpr std::uint64_t kTradeEngineId = 456;

constexpr std::uint64_t kInitialBalance = 100000 * kPriceMultiplier;

constexpr std::uint64_t kAaplPrice = 150 * kPriceMultiplier;
constexpr std::uint64_t kGooglPrice = 140 * kPriceMultiplier;
constexpr std::uint64_t kMsftPrice = 380 * kPriceMultiplier;

constexpr std::uint64_t kAaplBuyQty = 100;
constexpr std::uint64_t kGooglBuyQty = 50;
constexpr std::uint64_t kMsftBuyQty = 25;
constexpr std::uint64_t kAaplSellQty = 50;
}  // namespace

int main() {
  auto order_book = std::make_shared<OrderBook>(kOrderBookId);
  auto portfolio = std::make_shared<Portfolio>(kPortfolioId);

  order_book->SetPrice("AAPL", kAaplPrice);
  order_book->SetPrice("GOOGL", kGooglPrice);
  order_book->SetPrice("MSFT", kMsftPrice);

  portfolio->Deposit(kInitialBalance);
  std::cout << "Initial balance: " << FormatPrice(portfolio->GetBalance())
            << "\n\n";

  TradingEngine engine{kTradeEngineId};

  auto buy_aapl = std::make_unique<StockTradeCommand>(
      order_book, portfolio, Trade("AAPL", kAaplBuyQty, Side::kBuy));

  auto buy_googl = std::make_unique<StockTradeCommand>(
      order_book, portfolio, Trade("GOOGL", kGooglBuyQty, Side::kBuy));

  auto buy_msft = std::make_unique<StockTradeCommand>(
      order_book, portfolio, Trade("MSFT", kMsftBuyQty, Side::kBuy));

  std::cout << "=== Executing Trades ===\n";

  if (const auto kStatus = engine.SubmitOrder(std::move(buy_aapl));
      !kStatus.ok()) {
    std::cerr << "AAPL trade failed: " << kStatus.message() << "\n";
  }

  if (const auto kStatus = engine.SubmitOrder(std::move(buy_googl));
      !kStatus.ok()) {
    std::cerr << "GOOGL trade failed: " << kStatus.message() << "\n";
  }

  if (const auto kStatus = engine.SubmitOrder(std::move(buy_msft));
      !kStatus.ok()) {
    std::cerr << "MSFT trade failed: " << kStatus.message() << "\n";
  }

  std::cout << "\n=== Audit Log ===\n";
  for (const auto& entry : engine.GetAuditLog()) {
    std::cout << "  " << entry << "\n";
  }

  std::cout << "\n=== Undoing Last Trade (MSFT) ===\n";
  if (const auto kStatus = engine.UndoLast(); !kStatus.ok()) {
    std::cerr << "Undo failed: " << kStatus.message() << "\n";
  } else {
    std::cout << "Balance after undo: " << FormatPrice(portfolio->GetBalance())
              << "\n";
  }

  auto sell_aapl = std::make_unique<StockTradeCommand>(
      order_book, portfolio, Trade("AAPL", kAaplSellQty, Side::kSell));

  std::cout << "\n=== Selling 50 AAPL ===\n";
  if (const auto kStatus = engine.SubmitOrder(std::move(sell_aapl));
      !kStatus.ok()) {
    std::cerr << "Sell failed: " << kStatus.message() << "\n";
  }

  std::cout << "\n=== Final Audit Log ===\n";
  for (const auto& entry : engine.GetAuditLog()) {
    std::cout << "  " << entry << "\n";
  }

  std::cout << "\nFinal balance: " << FormatPrice(portfolio->GetBalance())
            << "\n";

  return 0;
}
