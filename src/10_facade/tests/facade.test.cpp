//
// Created by Will George on 11/10/25.
//

#include "facade.h"

#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <gtest/gtest.h>

#include "../helpers/StdoutCaptureGuard.h"

class FacadeSuite : public ::testing::Test {
 protected:
  static constexpr SymbolId kSymbol = 0;
  static constexpr std::string kAapl = "AAPL";
  static constexpr std::string kMsft = "MSFT";
  static constexpr std::int64_t kPrice = 100;
  static constexpr std::int64_t kQuantity = 1'000;
  static constexpr std::int64_t kLimit = 1'000'000;
};

TEST_F(FacadeSuite, SymbolTable_AddsSymbols_AssignsSequentialIds_NoNulls) {
  SymbolTable table;

  const SymbolId aapl = table.Add(kAapl);
  const SymbolId msft = table.Add(kMsft);
  const SymbolId again = table.Add(kAapl);

  EXPECT_EQ(aapl, 0u);
  EXPECT_EQ(msft, 1u);
  EXPECT_EQ(aapl, again);

  EXPECT_EQ(table.Text(aapl), kAapl);
  EXPECT_EQ(table.Text(msft), kMsft);
  EXPECT_EQ(table.to_text_.size(), 2u);

  EXPECT_EQ(std::string(table.Text(aapl)).find('\0'), std::string::npos);
}

TEST_F(FacadeSuite, PricingService_ReturnsPrice_WhenSymbolExists) {
  absl::flat_hash_map<SymbolId, std::int64_t> prices;
  prices.emplace(kSymbol, kPrice);

  const PricingService pricing(std::move(prices));

  StdoutCaptureGuard guard;
  auto price_or = pricing.GetFairPrice(kSymbol);
  const std::string out = guard.Capture();

  ASSERT_TRUE(price_or.ok());
  EXPECT_EQ(*price_or, kPrice);

  EXPECT_NE(out.find("priced @"), std::string::npos);
  EXPECT_EQ(out.find('\0'), std::string::npos);
}

TEST_F(FacadeSuite, PricingService_Fails_WhenSymbolMissing) {
  absl::flat_hash_map<SymbolId, std::int64_t> prices;
  const PricingService pricing(std::move(prices));

  constexpr SymbolId kUnknownSymbol = 42;

  const auto price_or = pricing.GetFairPrice(kUnknownSymbol);
  EXPECT_FALSE(price_or.ok());
  EXPECT_EQ(price_or.status().code(), absl::StatusCode::kNotFound);
}

TEST_F(FacadeSuite, RiskService_WithinLimit_Succeeds_WhenNotionalBelowLimit) {
  const RiskService risk(kLimit);

  constexpr Order order{
      .symbol_ = kSymbol, .price_ = kPrice, .quantity_ = kQuantity};

  absl::Status s = risk.WithinLimit(order);
  EXPECT_TRUE(s.ok());
}

TEST_F(FacadeSuite, RiskService_WithinLimit_Fails_WhenAboveLimit_Prints) {
  const RiskService risk(kLimit);

  constexpr std::int64_t kUnitPrice = 1;

  constexpr Order order{
      .symbol_ = kSymbol, .price_ = kUnitPrice, .quantity_ = kLimit + 1};

  StdoutCaptureGuard guard;
  const absl::Status isAllowed = risk.WithinLimit(order);
  const std::string out = guard.Capture();

  EXPECT_FALSE(isAllowed.ok());
  EXPECT_EQ(isAllowed.code(), absl::StatusCode::kResourceExhausted);

  EXPECT_NE(out.find("Risk:"), std::string::npos);
  EXPECT_EQ(out.find('\0'), std::string::npos);
}

TEST_F(FacadeSuite, ExecutionService_PrintsExecution_NoNulls) {
  SymbolTable table;
  const SymbolId aapl = table.Add(kAapl);

  ExecutionService exec(std::move(table));

  const Order kOrder{.symbol_ = aapl, .price_ = kPrice, .quantity_ = kQuantity};

  StdoutCaptureGuard guard;
  absl::Status s = exec.SendOrder(kOrder);
  const std::string out = guard.Capture();

  EXPECT_TRUE(s.ok());
  EXPECT_NE(out.find("EXECUTING BUY"), std::string::npos);
  EXPECT_NE(out.find(kAapl), std::string::npos);
  EXPECT_EQ(out.find('\0'), std::string::npos);
}

TEST_F(FacadeSuite, TradingFacade_HappyPath_Executes) {
  SymbolTable table;
  const SymbolId aapl = table.Add(kAapl);

  absl::flat_hash_map<SymbolId, std::int64_t> prices;
  prices.emplace(aapl, kPrice);

  PricingService pricing(std::move(prices));
  RiskService risk(kLimit);

  ExecutionService exec(std::move(table));
  TradingFacade facade(std::move(pricing), std::move(risk), std::move(exec));

  StdoutCaptureGuard guard;
  absl::Status s = facade.PlaceMarketOrder(aapl, kQuantity);
  const std::string out = guard.Capture();

  EXPECT_TRUE(s.ok());
  EXPECT_NE(out.find("EXECUTING BUY"), std::string::npos);
  EXPECT_EQ(out.find('\0'), std::string::npos);
}

TEST_F(FacadeSuite, TradingFacade_Fails_WhenSymbolHasNoPrice) {
  SymbolTable table;
  const SymbolId kAaplSym = table.Add(kAapl);

  absl::flat_hash_map<SymbolId, std::int64_t> prices;
  PricingService pricing(std::move(prices));
  RiskService risk(kLimit);

  ExecutionService exec(std::move(table));
  TradingFacade facade(std::move(pricing), std::move(risk), std::move(exec));

  absl::Status s = facade.PlaceMarketOrder(kAaplSym, 1);
  EXPECT_FALSE(s.ok());
  EXPECT_EQ(s.code(), absl::StatusCode::kNotFound);
}

TEST_F(FacadeSuite, TradingFacade_Fails_WhenRiskRejected) {
  SymbolTable table;
  const SymbolId aapl = table.Add(kAapl);

  absl::flat_hash_map<SymbolId, std::int64_t> prices;
  prices.emplace(aapl, kPrice);
  PricingService pricing(std::move(prices));

  constexpr std::int64_t kSmallLimit = 5'000;

  RiskService risk(kSmallLimit);

  ExecutionService exec(std::move(table));
  TradingFacade facade(std::move(pricing), std::move(risk), std::move(exec));

  absl::Status s = facade.PlaceMarketOrder(aapl, kQuantity);

  EXPECT_FALSE(s.ok());
  EXPECT_EQ(s.code(), absl::StatusCode::kResourceExhausted);
}
