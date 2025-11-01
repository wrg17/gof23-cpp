//
// Created by Will George on 10/24/25.
//

#include "composite.h"

#include <memory>

#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <gtest/gtest.h>

class CompositeSuite : public ::testing::Test {
 protected:
  static constexpr std::string kSymbol_ = "AAPL";
  static constexpr std::string kName_ = "Apple";

  static std::unique_ptr<RiskNode> MakeStock(const uint32_t kQty,
                                             const double kPrice = 100.0) {
    auto stock_or = Stock::Create(Ticker{kSymbol_, kName_}, kQty, kPrice);
    if (!stock_or.ok()) return nullptr;
    return std::move(stock_or).value();
  }
};

TEST_F(CompositeSuite, StockCreateShouldSucceedAndNetCostIsQuantity) {
  constexpr uint32_t kQty = 50;
  constexpr double kPrice = 100.0;
  constexpr double kNetCost = static_cast<double>(kQty) * kPrice;

  auto stock_or = Stock::Create(Ticker{kSymbol_, kName_}, kQty, kPrice);
  ASSERT_TRUE(stock_or.ok());
  const auto stock = std::move(stock_or).value();
  EXPECT_DOUBLE_EQ(stock->NetCost(), kNetCost);
}

TEST_F(CompositeSuite, StockAddShouldFailBecauseNotComposite) {
  constexpr uint32_t kQty = 1;
  constexpr double kPrice = 10.0;
  auto stock_or = Stock::Create(Ticker{kSymbol_, kName_}, kQty, kPrice);
  ASSERT_TRUE(stock_or.ok());
  const auto stock = std::move(stock_or).value();

  auto other = MakeStock(2);
  ASSERT_NE(other, nullptr);

  const absl::Status st = stock->Add(std::move(other));
  EXPECT_FALSE(st.ok());
  EXPECT_EQ(st.code(), absl::StatusCode::kFailedPrecondition);
  EXPECT_NE(st.message().find("not a composite"), std::string::npos);
}

TEST_F(CompositeSuite, PortfolioCreateShouldSucceedAndStartEmpty) {
  auto portfolio_or = Portfolio::Create();
  ASSERT_TRUE(portfolio_or.ok());
  const auto portfolio = std::move(portfolio_or).value();
  EXPECT_DOUBLE_EQ(portfolio->NetCost(), 0.0);
  EXPECT_TRUE(portfolio->Children().empty());
}

TEST_F(CompositeSuite, PortfolioAddNullShouldReturnInvalidArgument) {
  auto portfolio_or = Portfolio::Create();
  ASSERT_TRUE(portfolio_or.ok());
  const auto portfolio = std::move(portfolio_or).value();

  const absl::Status st = portfolio->Add(nullptr);
  EXPECT_FALSE(st.ok());
  EXPECT_EQ(st.code(), absl::StatusCode::kInvalidArgument);
}

TEST_F(CompositeSuite, PortfolioShouldAggregateChildNetCosts) {
  constexpr uint32_t kStockOneQty = 10;
  constexpr double kStockOnePrice = 100.0;

  constexpr uint32_t kStockTwoQty = 20;
  constexpr double kStockTwoPrice = 200.0;

  constexpr double kExpectedNetCost =
      kStockOneQty * kStockOnePrice + kStockTwoQty * kStockTwoPrice;
  constexpr uint32_t kExpectedChildren = 2;

  auto portfolio_or = Portfolio::Create();
  ASSERT_TRUE(portfolio_or.ok());
  const auto portfolio = std::move(portfolio_or).value();

  auto s1 = MakeStock(kStockOneQty, kStockOnePrice);
  auto s2 = MakeStock(kStockTwoQty, kStockTwoPrice);
  ASSERT_NE(s1, nullptr);
  ASSERT_NE(s2, nullptr);

  EXPECT_TRUE(portfolio->Add(std::move(s1)).ok());
  EXPECT_TRUE(portfolio->Add(std::move(s2)).ok());

  EXPECT_DOUBLE_EQ(portfolio->NetCost(), kExpectedNetCost);
  EXPECT_EQ(portfolio->Children().size(), kExpectedChildren);
}

TEST_F(CompositeSuite, PortfolioShouldAggregateNestedPortfolio) {
  auto parent_or = Portfolio::Create();
  ASSERT_TRUE(parent_or.ok());
  const auto parent = std::move(parent_or).value();

  auto child_or = Portfolio::Create();
  ASSERT_TRUE(child_or.ok());
  std::unique_ptr<RiskNode> child = std::move(child_or).value();

  constexpr uint32_t kStockOneQty = 5;
  constexpr double kStockOnePrice = 10.0;

  constexpr uint32_t kStockTwoQty = 7;
  constexpr double kStockTwoPrice = 20.0;

  constexpr double kExpectedNetCost = 190;

  auto s1 = MakeStock(kStockOneQty, kStockOnePrice);
  auto s2 = MakeStock(kStockTwoQty, kStockTwoPrice);
  ASSERT_NE(s1, nullptr);
  ASSERT_NE(s2, nullptr);

  ASSERT_TRUE(child->Add(std::move(s1)).ok());
  ASSERT_TRUE(parent->Add(std::move(s2)).ok());

  ASSERT_TRUE(parent->Add(std::move(child)).ok());

  EXPECT_DOUBLE_EQ(parent->NetCost(), kExpectedNetCost);
}