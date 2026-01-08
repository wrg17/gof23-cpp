//
// Created by Will George on 1/6/26.
//

#include "chain_of_responsibility.h"

#include <gtest/gtest.h>

#include "../helpers/StdoutCaptureGuard.h"

class ChainOfResponsibilitySuite : public ::testing::Test {
 protected:
  std::shared_ptr<IHandler> chain_ = std::make_shared<RiskHandler>(
      std::make_shared<ExecutionHandler>(nullptr));
};

TEST_F(ChainOfResponsibilitySuite, ShouldProcessValidBuyOrder) {
  const TradeRequest kValidBuy{
      .ticker = "AAPL", .quantity = 100, .price = 15000};

  const auto kResult = chain_->Operation(kValidBuy);

  EXPECT_TRUE(kResult.ok());
}

TEST_F(ChainOfResponsibilitySuite, ShouldProcessValidSellOrder) {
  const TradeRequest kValidSell{
      .ticker = "AAPL", .quantity = -100, .price = 15000};

  const auto kResult = chain_->Operation(kValidSell);

  EXPECT_TRUE(kResult.ok());
}

TEST_F(ChainOfResponsibilitySuite, ShouldLogRiskHandlerApproval) {
  const TradeRequest kValidBuy{
      .ticker = "MSFT", .quantity = 50, .price = 10000};

  StdoutCaptureGuard capture{};
  const absl::Status kStatus = chain_->Operation(kValidBuy);
  EXPECT_TRUE(kStatus.ok());

  const std::string kOutput = capture.Capture();
  EXPECT_NE(kOutput.find("RiskHandler: Trade within limits"),
            std::string::npos);
}

TEST_F(ChainOfResponsibilitySuite, ShouldLogExecutionSuccess) {
  const TradeRequest kValidBuy{
      .ticker = "GOOG", .quantity = 25, .price = 20000};

  StdoutCaptureGuard capture{};
  const absl::Status kStatus = chain_->Operation(kValidBuy);
  EXPECT_TRUE(kStatus.ok());

  const std::string kOutput = capture.Capture();
  EXPECT_NE(kOutput.find("Trade executed successfully"), std::string::npos);
}

TEST_F(ChainOfResponsibilitySuite, ShouldRejectInvalidTicker) {
  const TradeRequest kInvalidTicker{
      .ticker = "TSLA", .quantity = 50, .price = 7500};

  const auto result = chain_->Operation(kInvalidTicker);

  EXPECT_FALSE(result.ok());
  EXPECT_EQ(result.code(), absl::StatusCode::kAborted);
}

TEST_F(ChainOfResponsibilitySuite, ShouldRejectTradeExceedingRiskLimit) {
  const TradeRequest kRiskyTrade{
      .ticker = "MSFT", .quantity = 1'000'000, .price = 1'000'000'00};

  StdoutCaptureGuard capture{};

  const auto result = chain_->Operation(kRiskyTrade);
  const std::string output = capture.Capture();

  EXPECT_FALSE(result.ok());
  EXPECT_EQ(result.code(), absl::StatusCode::kFailedPrecondition);
  EXPECT_EQ(result.message(), "Risk limit exceeded");

  EXPECT_EQ(output.find("RiskHandler: Trade within limits"), std::string::npos);
  EXPECT_EQ(output.find("Trade executed successfully"), std::string::npos);
}

TEST_F(ChainOfResponsibilitySuite, ShouldHandleZeroQuantity) {
  const TradeRequest kZeroQuantity{
      .ticker = "AAPL", .quantity = 0, .price = 15000};

  const auto result = chain_->Operation(kZeroQuantity);

  EXPECT_TRUE(result.ok());
}

TEST_F(ChainOfResponsibilitySuite, ShouldHandleZeroPrice) {
  const TradeRequest kZeroPrice{.ticker = "MSFT", .quantity = 100, .price = 0};

  const auto result = chain_->Operation(kZeroPrice);

  EXPECT_TRUE(result.ok());
}

TEST_F(ChainOfResponsibilitySuite, ShouldHandleTradeAtExactRiskLimit) {
  const TradeRequest kAtLimit{
      .ticker = "AAPL", .quantity = 10'000, .price = 10'000'00};

  const auto result = chain_->Operation(kAtLimit);

  EXPECT_FALSE(result.ok());
}

class SingleHandlerSuite : public ::testing::Test {};

TEST_F(SingleHandlerSuite, ShouldReturnUnavailableWhenNoNextHandler) {
  const auto kTerminalHandler = std::make_shared<RiskHandler>(nullptr);

  const TradeRequest kValidTrade{
      .ticker = "AAPL", .quantity = 100, .price = 15000};

  const auto result = kTerminalHandler->Operation(kValidTrade);

  EXPECT_FALSE(result.ok());
  EXPECT_EQ(result.code(), absl::StatusCode::kUnavailable);
  EXPECT_EQ(result.message(), "No handler available for request");
}

TEST_F(SingleHandlerSuite, ShouldHandleExecutionHandlerAlone) {
  const auto execution_only = std::make_shared<ExecutionHandler>(nullptr);

  const TradeRequest kValidTrade{
      .ticker = "MSFT", .quantity = 100, .price = 15000};

  const auto result = execution_only->Operation(kValidTrade);

  EXPECT_TRUE(result.ok());
}

class TradeRequestOutputSuite : public ::testing::Test {};

TEST_F(TradeRequestOutputSuite, ShouldFormatBuyOrderCorrectly) {
  const TradeRequest kBuy{.ticker = "AAPL", .quantity = 100, .price = 15000};

  StdoutCaptureGuard capture{};
  std::cout << kBuy;

  const std::string output = capture.Capture();
  EXPECT_NE(output.find("ticker=AAPL"), std::string::npos);
  EXPECT_NE(output.find("type=Buy"), std::string::npos);
  EXPECT_NE(output.find("quantity=100"), std::string::npos);
  EXPECT_NE(output.find("price=$150.00"), std::string::npos);
}

TEST_F(TradeRequestOutputSuite, ShouldFormatSellOrderCorrectly) {
  const TradeRequest kSell{.ticker = "MSFT", .quantity = -50, .price = 20000};

  StdoutCaptureGuard capture{};
  std::cout << kSell;

  const std::string output = capture.Capture();
  EXPECT_NE(output.find("ticker=MSFT"), std::string::npos);
  EXPECT_NE(output.find("type=Sell"), std::string::npos);
  EXPECT_NE(output.find("quantity=50"), std::string::npos);
  EXPECT_NE(output.find("price=$200.00"), std::string::npos);
}
