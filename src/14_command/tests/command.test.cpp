//
// Created by Will George on 1/8/26.
//

#include "command.h"

#include <gtest/gtest.h>

#include "../../helpers/StdoutCaptureGuard.h"

class FormatPriceSuite : public ::testing::Test {};

TEST_F(FormatPriceSuite, ShouldFormatWholeNumber) {
  constexpr std::uint64_t kValue = 150 * kPriceMultiplier;

  const auto kResult = FormatPrice(kValue);

  EXPECT_EQ(kResult, "$150.0000");
}

TEST_F(FormatPriceSuite, ShouldFormatWithDecimalPlaces) {
  constexpr std::uint64_t kValue = 150'5025;

  const auto kResult = FormatPrice(kValue);

  EXPECT_EQ(kResult, "$150.5025");
}

TEST_F(FormatPriceSuite, ShouldFormatZero) {
  constexpr std::uint64_t kValue = 0;

  const auto kResult = FormatPrice(kValue);

  EXPECT_EQ(kResult, "$0.0000");
}

TEST_F(FormatPriceSuite, ShouldFormatSmallFraction) {
  constexpr std::uint64_t kValue = 1;

  const auto kResult = FormatPrice(kValue);

  EXPECT_EQ(kResult, "$0.0001");
}

class TradeSuite : public ::testing::Test {
 protected:
  static constexpr std::uint64_t kQuantity = 100;
  static constexpr std::uint64_t kHalfQuantity = 50;
  static constexpr std::string kTicker = "AAPL";
};

TEST_F(TradeSuite, ShouldConstructBuyTrade) {
  const Trade kTrade{kTicker, kQuantity, Side::kBuy};

  EXPECT_EQ(kTrade.GetTicker(), kTicker);
  EXPECT_EQ(kTrade.GetQuantity(), kQuantity);
  EXPECT_EQ(kTrade.GetSide(), Side::kBuy);
  EXPECT_EQ(kTrade.GetSideString(), "Buy");
}

TEST_F(TradeSuite, ShouldConstructSellTrade) {
  const Trade kTrade{kTicker, kQuantity, Side::kSell};

  EXPECT_EQ(kTrade.GetTicker(), kTicker);
  EXPECT_EQ(kTrade.GetQuantity(), kQuantity);
  EXPECT_EQ(kTrade.GetSide(), Side::kSell);
  EXPECT_EQ(kTrade.GetSideString(), "Sell");
}

TEST_F(TradeSuite, ShouldGetInverseOfBuy) {
  const Trade kBuy{kTicker, kQuantity, Side::kBuy};

  const Trade kInverse = kBuy.GetInverse();

  EXPECT_EQ(kInverse.GetTicker(), kTicker);
  EXPECT_EQ(kInverse.GetQuantity(), kQuantity);
  EXPECT_EQ(kInverse.GetSide(), Side::kSell);
}

TEST_F(TradeSuite, ShouldGetInverseOfSell) {
  const Trade kSell{kTicker, kQuantity, Side::kSell};

  const Trade kInverse = kSell.GetInverse();

  EXPECT_EQ(kInverse.GetTicker(), kTicker);
  EXPECT_EQ(kInverse.GetQuantity(), kQuantity);
  EXPECT_EQ(kInverse.GetSide(), Side::kBuy);
}

TEST_F(TradeSuite, ShouldFormatToString) {
  const Trade kTrade{kTicker, kQuantity, Side::kBuy};

  const auto kResult = kTrade.ToString();

  EXPECT_NE(kResult.find("ticker=AAPL"), std::string::npos);
  EXPECT_NE(kResult.find("type=Buy"), std::string::npos);
  EXPECT_NE(kResult.find("quantity=100"), std::string::npos);
}

TEST_F(TradeSuite, ShouldOutputToStream) {
  const Trade kTrade{"MSFT", kHalfQuantity, Side::kSell};

  StdoutCaptureGuard capture{};
  std::cout << kTrade;

  const std::string kOutput = capture.Capture();

  EXPECT_NE(kOutput.find("ticker=MSFT"), std::string::npos);
  EXPECT_NE(kOutput.find("type=Sell"), std::string::npos);
  EXPECT_NE(kOutput.find("quantity="), std::string::npos);
}

class PortfolioSuite : public ::testing::Test {
 protected:
  static constexpr std::uint64_t kPortfolioId = 12345;
  static constexpr std::uint64_t kInitialBalance = 100'000 * kPriceMultiplier;
  static constexpr std::uint64_t kSmallBalance = 1'000 * kPriceMultiplier;
  static constexpr std::uint64_t kPrice = 150 * kPriceMultiplier;
  static constexpr std::uint64_t kQuantity = 100;
  static constexpr std::uint64_t kHalfQuantity = 50;
};

TEST_F(PortfolioSuite, ShouldConstructWithId) {
  const Portfolio kPortfolio{kPortfolioId};

  EXPECT_EQ(kPortfolio.GetId(), kPortfolioId);
  EXPECT_EQ(kPortfolio.GetBalance(), 0);
}

TEST_F(PortfolioSuite, ShouldDeposit) {
  Portfolio portfolio{kPortfolioId};

  const auto kNewBalance = portfolio.Deposit(kInitialBalance);

  EXPECT_EQ(kNewBalance, kInitialBalance);
  EXPECT_EQ(portfolio.GetBalance(), kInitialBalance);
}

TEST_F(PortfolioSuite, ShouldAccumulateDeposits) {
  Portfolio portfolio{kPortfolioId};
  constexpr std::uint64_t kFirstDeposit = 50'000 * kPriceMultiplier;
  constexpr std::uint64_t kSecondDeposit = 30'000 * kPriceMultiplier;

  portfolio.Deposit(kFirstDeposit);
  portfolio.Deposit(kSecondDeposit);

  EXPECT_EQ(portfolio.GetBalance(), kFirstDeposit + kSecondDeposit);
}

TEST_F(PortfolioSuite, ShouldBuyHolding) {
  Portfolio portfolio{kPortfolioId};
  portfolio.Deposit(kInitialBalance);
  const Trade kBuy{"AAPL", kQuantity, Side::kBuy};

  const auto kStatus = portfolio.UpdateHolding(kBuy, kPrice);

  EXPECT_TRUE(kStatus.ok());
  constexpr std::uint64_t kExpectedBalance =
      kInitialBalance - (kQuantity * kPrice);
  EXPECT_EQ(portfolio.GetBalance(), kExpectedBalance);
}

TEST_F(PortfolioSuite, ShouldRejectBuyWithInsufficientFunds) {
  Portfolio portfolio{kPortfolioId};
  portfolio.Deposit(kSmallBalance);
  const Trade kBuy{"AAPL", kQuantity, Side::kBuy};

  const auto kStatus = portfolio.UpdateHolding(kBuy, kPrice);

  EXPECT_FALSE(kStatus.ok());
  EXPECT_EQ(kStatus.code(), absl::StatusCode::kResourceExhausted);
  EXPECT_NE(kStatus.message().find("Insufficient funds"), std::string::npos);
}

TEST_F(PortfolioSuite, ShouldSellHolding) {
  Portfolio portfolio{kPortfolioId};
  portfolio.Deposit(kInitialBalance);
  const Trade kBuy{"AAPL", kQuantity, Side::kBuy};
  (void)portfolio.UpdateHolding(kBuy, kPrice);
  const auto kBalanceAfterBuy = portfolio.GetBalance();

  const Trade kSell{"AAPL", kHalfQuantity, Side::kSell};
  const auto kStatus = portfolio.UpdateHolding(kSell, kPrice);

  EXPECT_TRUE(kStatus.ok());
  EXPECT_EQ(portfolio.GetBalance(),
            kBalanceAfterBuy + (kHalfQuantity * kPrice));
}

TEST_F(PortfolioSuite, ShouldRejectSellWithNoHolding) {
  Portfolio portfolio{kPortfolioId};
  portfolio.Deposit(kInitialBalance);
  const Trade kSell{"AAPL", kQuantity, Side::kSell};

  const auto kStatus = portfolio.UpdateHolding(kSell, kPrice);

  EXPECT_FALSE(kStatus.ok());
  EXPECT_EQ(kStatus.code(), absl::StatusCode::kNotFound);
  EXPECT_NE(kStatus.message().find("short"), std::string::npos);
}

TEST_F(PortfolioSuite, ShouldRejectSellExceedingHolding) {
  Portfolio portfolio{kPortfolioId};
  portfolio.Deposit(kInitialBalance);
  const Trade kBuy{"AAPL", kHalfQuantity, Side::kBuy};
  (void)portfolio.UpdateHolding(kBuy, kPrice);

  const Trade kSell{"AAPL", kQuantity, Side::kSell};
  const auto kStatus = portfolio.UpdateHolding(kSell, kPrice);

  EXPECT_FALSE(kStatus.ok());
  EXPECT_EQ(kStatus.code(), absl::StatusCode::kResourceExhausted);
}

class OrderBookSuite : public ::testing::Test {
 protected:
  static constexpr std::uint64_t kOrderBookId = 54321;
  static constexpr std::uint64_t kPortfolioId = 12345;
  static constexpr std::uint64_t kInitialBalance = 100'000 * kPriceMultiplier;
  static constexpr std::uint64_t kPrice = 150 * kPriceMultiplier;
  static constexpr std::uint64_t kQuantity = 100;
};

TEST_F(OrderBookSuite, ShouldSetAndGetPrice) {
  OrderBook order_book{kOrderBookId};

  order_book.SetPrice("AAPL", kPrice);
  const auto kResult = order_book.GetPrice("AAPL");

  ASSERT_TRUE(kResult.ok());
  EXPECT_EQ(*kResult, kPrice);
}

TEST_F(OrderBookSuite, ShouldReturnErrorForUnknownTicker) {
  OrderBook order_book{kOrderBookId};

  const auto kResult = order_book.GetPrice("UNKNOWN");

  EXPECT_FALSE(kResult.ok());
  EXPECT_EQ(kResult.status().code(), absl::StatusCode::kNotFound);
  EXPECT_NE(kResult.status().message().find("No price data"),
            std::string::npos);
}

TEST_F(OrderBookSuite, ShouldExecuteBuyTrade) {
  OrderBook order_book{kOrderBookId};
  order_book.SetPrice("AAPL", kPrice);

  auto portfolio = std::make_shared<Portfolio>(kPortfolioId);
  portfolio->Deposit(kInitialBalance);

  const Trade kBuy{"AAPL", kQuantity, Side::kBuy};

  const auto kStatus = order_book.ExecuteTrade(portfolio, kBuy);

  EXPECT_TRUE(kStatus.ok());
}

TEST_F(OrderBookSuite, ShouldFailTradeForUnknownTicker) {
  OrderBook order_book{kOrderBookId};
  auto portfolio = std::make_shared<Portfolio>(kPortfolioId);
  portfolio->Deposit(kInitialBalance);
  const Trade kBuy{"UNKNOWN", kQuantity, Side::kBuy};

  const auto kStatus = order_book.ExecuteTrade(portfolio, kBuy);

  EXPECT_FALSE(kStatus.ok());
  EXPECT_EQ(kStatus.code(), absl::StatusCode::kNotFound);
}

class StockTradeCommandSuite : public ::testing::Test {
 protected:
  static constexpr std::uint64_t kOrderBookId = 54321;
  static constexpr std::uint64_t kPortfolioId = 12345;
  static constexpr std::uint64_t kPrice = 150 * kPriceMultiplier;
  static constexpr std::uint64_t kInitialBalance = 100'000 * kPriceMultiplier;
  static constexpr std::uint64_t kQuantity = 100;
  static constexpr std::uint64_t kHalfQuantity = 50;

  std::shared_ptr<OrderBook> order_book_ =
      std::make_shared<OrderBook>(kOrderBookId);
  std::shared_ptr<Portfolio> portfolio_ =
      std::make_shared<Portfolio>(kPortfolioId);

  void SetUp() override {
    order_book_->SetPrice("AAPL", kPrice);
    portfolio_->Deposit(kInitialBalance);
  }
};

TEST_F(StockTradeCommandSuite, ShouldExecuteBuyCommand) {
  StockTradeCommand command{order_book_, portfolio_,
                            Trade{"AAPL", kQuantity, Side::kBuy}};

  const auto kStatus = command.Execute();

  EXPECT_TRUE(kStatus.ok());
  constexpr std::uint64_t kExpected = kInitialBalance - (kQuantity * kPrice);
  EXPECT_EQ(portfolio_->GetBalance(), kExpected);
}

TEST_F(StockTradeCommandSuite, ShouldUndoBuyCommand) {
  StockTradeCommand command{order_book_, portfolio_,
                            Trade{"AAPL", kQuantity, Side::kBuy}};
  (void)command.Execute();

  const auto kStatus = command.Undo();

  EXPECT_TRUE(kStatus.ok());
  EXPECT_EQ(portfolio_->GetBalance(), kInitialBalance);
}

TEST_F(StockTradeCommandSuite, ShouldRejectDoubleExecute) {
  StockTradeCommand command{order_book_, portfolio_,
                            Trade{"AAPL", kQuantity, Side::kBuy}};
  (void)command.Execute();

  const auto kStatus = command.Execute();

  EXPECT_FALSE(kStatus.ok());
  EXPECT_EQ(kStatus.code(), absl::StatusCode::kFailedPrecondition);
  EXPECT_NE(kStatus.message().find("Already executed"), std::string::npos);
}

TEST_F(StockTradeCommandSuite, ShouldRejectUndoWithoutExecute) {
  StockTradeCommand command{order_book_, portfolio_,
                            Trade{"AAPL", kQuantity, Side::kBuy}};

  const auto kStatus = command.Undo();

  EXPECT_FALSE(kStatus.ok());
  EXPECT_EQ(kStatus.code(), absl::StatusCode::kFailedPrecondition);
  EXPECT_NE(kStatus.message().find("Not executed"), std::string::npos);
}

TEST_F(StockTradeCommandSuite, ShouldProvideDescription) {
  StockTradeCommand command{order_book_, portfolio_,
                            Trade{"AAPL", kQuantity, Side::kBuy}};
  (void)command.Execute();

  const auto kDescription = command.Description();

  EXPECT_NE(kDescription.find("Portfolio 12345"), std::string::npos);
  EXPECT_NE(kDescription.find("AAPL"), std::string::npos);
  EXPECT_NE(kDescription.find("REMAINING BALANCE"), std::string::npos);
}

TEST_F(StockTradeCommandSuite, ShouldExecuteSellCommand) {
  StockTradeCommand buy_command{order_book_, portfolio_,
                                Trade{"AAPL", kQuantity, Side::kBuy}};
  (void)buy_command.Execute();
  const auto kBalanceAfterBuy = portfolio_->GetBalance();

  StockTradeCommand sell_command{order_book_, portfolio_,
                                 Trade{"AAPL", kHalfQuantity, Side::kSell}};
  const auto kStatus = sell_command.Execute();

  EXPECT_TRUE(kStatus.ok());
  EXPECT_EQ(portfolio_->GetBalance(),
            kBalanceAfterBuy + (kHalfQuantity * kPrice));
}

class TradingEngineSuite : public ::testing::Test {
 protected:
  static constexpr std::uint64_t kOrderBookId = 54321;
  static constexpr std::uint64_t kPortfolioId = 12345;
  static constexpr std::uint64_t kTradingEngineId = 456;
  static constexpr std::uint64_t kPrice = 150 * kPriceMultiplier;
  static constexpr std::uint64_t kMsftPrice = 380 * kPriceMultiplier;
  static constexpr std::uint64_t kInitialBalance = 100'000 * kPriceMultiplier;
  static constexpr std::uint64_t kQuantity = 100;
  static constexpr std::uint64_t kHalfQuantity = 50;
  static constexpr std::uint64_t kSmallQuantity = 25;
  static constexpr std::uint64_t kLargeQuantity = 10'000;

  std::shared_ptr<OrderBook> order_book_ =
      std::make_shared<OrderBook>(kOrderBookId);
  std::shared_ptr<Portfolio> portfolio_ =
      std::make_shared<Portfolio>(kPortfolioId);
  TradingEngine engine_{kTradingEngineId};

  void SetUp() override {
    order_book_->SetPrice("AAPL", kPrice);
    order_book_->SetPrice("MSFT", kMsftPrice);
    portfolio_->Deposit(kInitialBalance);
  }

  std::unique_ptr<StockTradeCommand> MakeBuyCommand(const std::string& ticker,
                                                    std::uint64_t quantity) {
    return std::make_unique<StockTradeCommand>(
        order_book_, portfolio_, Trade{ticker, quantity, Side::kBuy});
  }
};

TEST_F(TradingEngineSuite, ShouldSubmitOrder) {
  auto command = MakeBuyCommand("AAPL", kQuantity);

  const auto kStatus = engine_.SubmitOrder(std::move(command));

  EXPECT_TRUE(kStatus.ok());
  EXPECT_EQ(engine_.GetAuditLog().size(), 1);
}

TEST_F(TradingEngineSuite, ShouldRecordAuditLog) {
  (void)engine_.SubmitOrder(MakeBuyCommand("AAPL", kQuantity));
  (void)engine_.SubmitOrder(MakeBuyCommand("MSFT", kSmallQuantity));

  const auto& log = engine_.GetAuditLog();

  EXPECT_EQ(log.size(), 2);
  EXPECT_NE(log[0].find("AAPL"), std::string::npos);
  EXPECT_NE(log[1].find("MSFT"), std::string::npos);
}

TEST_F(TradingEngineSuite, ShouldUndoLastOrder) {
  (void)engine_.SubmitOrder(MakeBuyCommand("AAPL", kQuantity));
  const auto kBalanceAfterBuy = portfolio_->GetBalance();
  (void)engine_.SubmitOrder(MakeBuyCommand("MSFT", kSmallQuantity));

  const auto kStatus = engine_.UndoLast();

  EXPECT_TRUE(kStatus.ok());
  EXPECT_EQ(portfolio_->GetBalance(), kBalanceAfterBuy);
}

TEST_F(TradingEngineSuite, ShouldRejectUndoWithNoHistory) {
  const auto kStatus = engine_.UndoLast();

  EXPECT_FALSE(kStatus.ok());
  EXPECT_EQ(kStatus.code(), absl::StatusCode::kFailedPrecondition);
  EXPECT_NE(kStatus.message().find("No commands"), std::string::npos);
}

TEST_F(TradingEngineSuite, ShouldRejectFailedOrder) {
  auto command = MakeBuyCommand("AAPL", kLargeQuantity);

  const auto kStatus = engine_.SubmitOrder(std::move(command));

  EXPECT_FALSE(kStatus.ok());
  EXPECT_EQ(engine_.GetAuditLog().size(), 0);
}

TEST_F(TradingEngineSuite, ShouldUndoMultipleOrders) {
  (void)engine_.SubmitOrder(MakeBuyCommand("AAPL", kHalfQuantity));
  (void)engine_.SubmitOrder(MakeBuyCommand("AAPL", kHalfQuantity));

  (void)engine_.UndoLast();
  (void)engine_.UndoLast();

  EXPECT_EQ(portfolio_->GetBalance(), kInitialBalance);
}

TEST_F(TradingEngineSuite, ShouldRecordUndoInAuditLog) {
  (void)engine_.SubmitOrder(MakeBuyCommand("AAPL", kQuantity));
  EXPECT_EQ(engine_.GetAuditLog().size(), 1);

  (void)engine_.UndoLast();

  const auto& log = engine_.GetAuditLog();
  EXPECT_EQ(log.size(), 2);
  EXPECT_NE(log[1].find("UNDO:"), std::string::npos);
  EXPECT_NE(log[1].find("AAPL"), std::string::npos);
}
