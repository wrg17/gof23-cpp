//
// Created by Will George on 11/6/25.
//

#include "decorator.h"

#include <memory>

#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <gtest/gtest.h>

#include "../helpers/StdoutCaptureGuard.h"

class DecoratorSuite : public ::testing::Test {};

TEST_F(DecoratorSuite, ExchangeService_PrintsOrder_NoNulls_LeftAligned) {
  constexpr Order order{
      .symbol_ = {"AAPL"},
      .price_ = 2684700,
      .quantity_ = 100,
  };

  std::unique_ptr<IOrderService> service = std::make_unique<ExchangeOrderService>();

  StdoutCaptureGuard guard;
  service->Execute(order);
  const std::string out = guard.Capture();

  const std::string expected = "[ORDER] AAPL : $268.4700 x 100\n";
  EXPECT_EQ(out, expected);
  EXPECT_EQ(out.find('\0'), std::string::npos);
}

TEST_F(DecoratorSuite, LoggingDecorator_WrapsAndLogs_NoNulls) {
  constexpr Order order{
      .symbol_ = {"AAPL"},
      .price_ = 2684700,
      .quantity_ = 100,
  };

  std::unique_ptr<IOrderService> service = std::make_unique<ExchangeOrderService>();
  service = std::make_unique<LoggingDecoratorService>(std::move(service));

  StdoutCaptureGuard guard;
  service->Execute(order);
  const std::string out = guard.Capture();

  const std::string expected =
      "[LOG  ] Placing order\n"
      "[ORDER] AAPL : $268.4700 x 100\n"
      "[LOG  ] Completed order\n";

  EXPECT_EQ(out, expected);
  EXPECT_EQ(out.find('\0'), std::string::npos);
}