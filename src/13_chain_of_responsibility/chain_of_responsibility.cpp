//
// Created by Will George on 1/6/26.
//

#include "chain_of_responsibility.h"

#include <algorithm>
#include <array>
#include <format>
#include <iostream>

std::ostream& operator<<(std::ostream& os, const TradeRequest& request) {
  constexpr std::uint64_t kPriceMultiplier = 100;

  std::uint64_t dollars = request.price / kPriceMultiplier;
  std::uint64_t cents = request.price % kPriceMultiplier;

  std::string_view order_type = request.quantity > 0 ? "Buy" : "Sell";

  os << std::format(
      "TradeRequest{{ticker={}, type={}, quantity={}, price=${}.{:02}}}",
      request.ticker, order_type, std::abs(request.quantity), dollars, cents);
  return os;
}

IHandler::IHandler(std::shared_ptr<IHandler> next) : next_(std::move(next)) {}

absl::Status IHandler::PassToNext(const TradeRequest& request) const {
  if (next_) {
    return next_->Operation(request);
  }
  return absl::UnavailableError("No handler available for request");
}

RiskHandler::RiskHandler(std::shared_ptr<IHandler> next)
    : IHandler(std::move(next)) {}

absl::Status RiskHandler::Operation(const TradeRequest& request) {
  if (!IsHandled(request)) {
    return absl::FailedPreconditionError("Risk limit exceeded");
  }

  std::cout << "RiskHandler: Trade within limits, passing to next handler\n";

  return PassToNext(request);
}

bool RiskHandler::IsHandled(const TradeRequest& request) const {
  constexpr std::uint64_t kRiskLimit = 1'000'000'00;

  const auto abs_quantity =
      static_cast<std::uint64_t>(std::abs(request.quantity));
  const std::uint64_t total_value = request.price * abs_quantity;

  return total_value < kRiskLimit;
}

ExecutionHandler::ExecutionHandler(std::shared_ptr<IHandler> next)
    : IHandler(std::move(next)) {}

absl::Status ExecutionHandler::Operation(const TradeRequest& request) {
  if (!IsHandled(request)) {
    return absl::AbortedError("The trade is invalid");
  }

  std::cout << "Trade executed successfully\n";
  std::cout << "=========================\n";
  std::cout << request << "\n";
  std::cout << "=========================\n";

  return absl::OkStatus();
}

bool ExecutionHandler::IsHandled(const TradeRequest& request) const {
  constexpr std::array<std::string_view, 3> kValidTickers = {"AAPL", "MSFT",
                                                             "GOOG"};

  return std::ranges::find(kValidTickers, request.ticker) !=
         kValidTickers.end();
}