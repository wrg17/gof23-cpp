//
// Created by Will George on 1/8/26.
//

#include "command.h"

#include <format>

std::string FormatPrice(const std::uint64_t kValue) {
  const auto kWhole = kValue / kPriceMultiplier;
  const auto kFrac = kValue % kPriceMultiplier;
  return std::format("${}.{:04}", kWhole, kFrac);
}

Trade::Trade(std::string ticker, const std::uint64_t kQuantity,
             const Side kSide)
    : ticker_(std::move(ticker)), quantity_(kQuantity), side_(kSide) {}

const std::string& Trade::GetTicker() const { return ticker_; }

std::uint64_t Trade::GetQuantity() const { return quantity_; }

Side Trade::GetSide() const { return side_; }

std::string Trade::GetSideString() const {
  return side_ == Side::kBuy ? "Buy" : "Sell";
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
std::string Trade::ToString() const {
  auto order_type = GetSideString();
  return std::format("TradeRequest{{ticker={}, type={}, quantity={}}}", ticker_,
                     order_type, quantity_);
}

Trade Trade::GetInverse() const {
  return {GetTicker(), GetQuantity(),
          GetSide() == Side::kBuy ? Side::kSell : Side::kBuy};
}

std::ostream& operator<<(std::ostream& out, const Trade& request) {
  out << request.ToString();
  return out;
}

// Portfolio

Portfolio::Portfolio(const std::uint64_t kId) : id_(kId) {}

absl::Status Portfolio::BuyHolding(const Trade& trade,
                                   const std::uint64_t kPrice) {
  const std::string& ticker = trade.GetTicker();
  const uint64_t kBuyQty = trade.GetQuantity();

  const std::uint64_t kAmount = kPrice * kBuyQty;

  if (balance_ < kAmount) {
    return absl::ResourceExhaustedError(std::format(
        "Insufficient funds to buy {} shares of {}", kBuyQty, ticker));
  }

  balance_ -= kAmount;

  auto [iter, inserted] = holdings_.try_emplace(ticker, kBuyQty, kPrice);

  Holding& holding = iter->second;
  const auto [kInitQty, kInitAvgCost] = holding;

  if (!inserted) holding.quantity += kBuyQty;
  holding.avg_cost = ((kInitAvgCost * kInitQty) + kAmount) / holding.quantity;

  return absl::OkStatus();
}

absl::Status Portfolio::SellHolding(const Trade& trade,
                                    const std::uint64_t kPrice) {
  const std::string& ticker = trade.GetTicker();
  const uint64_t kSellQty = trade.GetQuantity();

  const auto kIter = holdings_.find(ticker);
  if (kIter == holdings_.end()) {
    return absl::NotFoundError(std::format(
        "Selling short is not allowed: {} (0 < {})", ticker, kSellQty));
  }

  Holding& holding = kIter->second;

  const auto [kInitQty, kInitAvgCost] = holding;

  if (kInitQty < kSellQty) {
    return absl::ResourceExhaustedError(
        std::format("Selling short is not allowed: {} ({} < {})", ticker,
                    kInitQty, kSellQty));
  }

  const std::uint64_t kAmount = kPrice * kSellQty;

  balance_ += kAmount;

  if (holding.quantity == kSellQty) {
    holdings_.erase(kIter);
    return absl::OkStatus();
  }

  holding.quantity -= kSellQty;

  return absl::OkStatus();
}

absl::Status Portfolio::UpdateHolding(const Trade& trade,
                                      const std::uint64_t kPrice) {
  switch (trade.GetSide()) {
    case Side::kBuy:
      return BuyHolding(trade, kPrice);
    case Side::kSell:
      return SellHolding(trade, kPrice);
  }
  return absl::NotFoundError("Invalid trade side");
}

std::uint64_t Portfolio::GetBalance() const { return balance_; }

std::uint64_t Portfolio::Deposit(const std::uint64_t kAmount) {
  balance_ += kAmount;
  return balance_;
}

std::uint64_t Portfolio::GetId() const { return id_; }

// OrderBook

OrderBook::OrderBook(const std::uint64_t kId) : id_(kId) {}

absl::Status OrderBook::ExecuteTrade(
    const std::shared_ptr<Portfolio>& portfolio, const Trade& trade) const {
  const auto kPriceOrStatus = GetPrice(trade.GetTicker());
  if (!kPriceOrStatus.ok()) {
    return kPriceOrStatus.status();
  }

  const std::uint64_t kUnitPrice = kPriceOrStatus.value();

  return portfolio->UpdateHolding(trade, kUnitPrice);
}

void OrderBook::SetPrice(const std::string& ticker,
                         const std::uint64_t kPrice) {
  prices_[ticker] = kPrice;
}

absl::StatusOr<std::uint64_t> OrderBook::GetPrice(
    const std::string& ticker) const {
  const auto kPriceIter = prices_.find(ticker);

  if (kPriceIter == prices_.end()) {
    return absl::NotFoundError(std::format(
        "OrderBook {}: No price data available for {}", id_, ticker));
  }
  return kPriceIter->second;
}

// ICommand

absl::Status ICommand::Execute() {
  if (executed_) return absl::FailedPreconditionError("Already executed");
  auto status = DoExecute();
  if (status.ok()) executed_ = true;
  return status;
}

absl::Status ICommand::Undo() {
  if (!executed_) return absl::FailedPreconditionError("Not executed");
  auto status = DoUndo();
  if (status.ok()) executed_ = false;
  return status;
}

// StockTradeCommand

StockTradeCommand::StockTradeCommand(std::shared_ptr<OrderBook> order_book,
                                     std::shared_ptr<Portfolio> portfolio,
                                     Trade trade)
    : order_book_(std::move(order_book)),
      portfolio_(std::move(portfolio)),
      trade_(std::move(trade)) {}

std::string StockTradeCommand::Description() const {
  return std::format("Portfolio {}: {} REMAINING BALANCE = {}",
                     portfolio_->GetId(), trade_.ToString(),
                     FormatPrice(portfolio_->GetBalance()));
}

absl::Status StockTradeCommand::DoExecute() {
  return order_book_->ExecuteTrade(portfolio_, trade_);
}

absl::Status StockTradeCommand::DoUndo() {
  return order_book_->ExecuteTrade(portfolio_, trade_.GetInverse());
}

// TradingEngine

TradingEngine::TradingEngine(const std::uint64_t kId) : id_(kId) {}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
absl::Status TradingEngine::SubmitOrder(std::unique_ptr<ICommand> command) {
  auto status = command->Execute();
  if (!status.ok()) return status;

  audit_log_.push_back(command->Description());
  history_.push_back(std::move(command));

  return absl::OkStatus();
}

absl::Status TradingEngine::UndoLast() {
  if (history_.empty()) {
    return absl::FailedPreconditionError(
        std::format("TradingEngine {}: No commands to undo", id_));
  }
  const std::unique_ptr<ICommand> kCommandPtr = std::move(history_.back());
  history_.pop_back();

  const auto kStatus = kCommandPtr->Undo();
  if (kStatus.ok()) {
    audit_log_.push_back(std::format("UNDO: {}", kCommandPtr->Description()));
  }
  return kStatus;
}

const std::vector<std::string>& TradingEngine::GetAuditLog() const {
  return audit_log_;
}
