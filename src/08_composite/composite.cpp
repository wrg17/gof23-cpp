//
// Created by Will George on 10/24/25.
//

#include "composite.h"

#include <ostream>
#include <string>
#include <utility>

Ticker::Ticker(std::string symbol, std::string name)
    : symbol_(std::move(symbol)), name_(std::move(name)) {}

const std::string& Ticker::Symbol() const { return symbol_; }

const std::string& Ticker::Name() const { return name_; }

absl::Status RiskNode::Add(std::unique_ptr<RiskNode> /*unused*/) {
  return absl::FailedPreconditionError("node is not a composite");
}

absl::StatusOr<std::unique_ptr<Stock>> Stock::Create(Ticker ticker,
                                                     const uint32_t kQuantity,
                                                     const double kPrice) {
  return std::make_unique<Stock>(Stock{std::move(ticker), kQuantity, kPrice});
}

double Stock::NetCost() const {
  return static_cast<double>(quantity_) * price_;
}

std::string Stock::ToString() const {
  return absl::StrFormat("%s (%s) @ $%.4f x %u", ticker_.Name(),
                         ticker_.Symbol(), price_, quantity_);
}

std::string Portfolio::ToString() const {
  std::string result = "Portfolio:\n";
  for (const auto& node : children_) {
    result += node->ToString() + '\n';
  }
  return result;
}

Stock::Stock(Ticker ticker, const uint32_t kQuantity, const double kPrice)
    : ticker_(std::move(ticker)), quantity_(kQuantity), price_(kPrice) {}

absl::StatusOr<std::unique_ptr<Portfolio>> Portfolio::Create() {
  return std::make_unique<Portfolio>(Portfolio{});
}

absl::Status Portfolio::Add(std::unique_ptr<RiskNode> node) {
  if (!node) return absl::InvalidArgumentError("null child");
  children_.emplace_back(std::move(node));
  return absl::OkStatus();
}

double Portfolio::NetCost() const {
  double sum = 0;
  for (const auto& node : children_) {
    sum += node->NetCost();
  }
  return sum;
}

const std::vector<std::unique_ptr<RiskNode>>& Portfolio::Children() const {
  return children_;
}

Portfolio::Portfolio() = default;