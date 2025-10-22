//
// Created by Will George on 10/20/25.
//

#include "adapter.h"

#include <memory>

#include <gtest/gtest.h>

#include "../helpers/StdoutCaptureGuard.h"

class AdapterSuite : public ::testing::Test {
 protected:
  static constexpr std::string kSym = "AAPL";
  static constexpr uint32_t kQty = 5;
  static constexpr double kPrice = 100.0;
};

class FixSessionSuite : public AdapterSuite {
 protected:
  static constexpr char kFixSide = 'B';
  static constexpr OrderId kOrderId = "FIX-ORDER-ID";
};

TEST_F(FixSessionSuite, ShouldReturnTheOrderId) {
  const auto kId =
      FixLib::Session::NewOrderSingle(kSym, kFixSide, kQty, kPrice);
  EXPECT_EQ(kId, kOrderId);
}

TEST_F(FixSessionSuite, ShouldLogTheOrder) {
  StdoutCaptureGuard capture_log{};
  FixLib::Session::NewOrderSingle(kSym, kFixSide, kQty, kPrice);
  EXPECT_EQ(capture_log.Capture(), "FIX: AAPL B 5 @ 100 -> FIX-ORDER-ID\n");
}

class RestClientSuite : public AdapterSuite {
 protected:
  static constexpr std::string kRestSide = "Buy";
  static constexpr std::string kTimeInForce = "GTC";
  static constexpr OrderId kOrderId = "REST-ORDER-ID";
};

TEST_F(RestClientSuite, ShouldReturnTheOrderId) {
  const auto kId =
      RestLib::Client::PostOrder(kSym, kRestSide, kQty, kPrice, kTimeInForce);
  EXPECT_EQ(kId, kOrderId);
}

TEST_F(RestClientSuite, ShouldLogTheOrder) {
  StdoutCaptureGuard capture_log{};
  RestLib::Client::PostOrder(kSym, kRestSide, kQty, kPrice, kTimeInForce);
  EXPECT_EQ(capture_log.Capture(),
            "REST: AAPL Buy 5 @ 100 for GTC -> REST-ORDER-ID\n");
}

class FixRouterSuite : public AdapterSuite {
 protected:
  std::shared_ptr<FixLib::Session> sess_ = std::make_shared<FixLib::Session>();
  std::unique_ptr<FixRouter> router_ = std::make_unique<FixRouter>(sess_);
};

TEST_F(FixRouterSuite, ShouldSendSellOrder) {
  StdoutCaptureGuard capture_log{};
  router_->SendLimit(kSym, Side::kSell, kPrice, kQty);
  EXPECT_EQ(capture_log.Capture(), "FIX: AAPL S 5 @ 100 -> FIX-ORDER-ID\n");
}

class RestRouterSuite : public AdapterSuite {
 protected:
  std::shared_ptr<RestLib::Client> client_ =
      std::make_shared<RestLib::Client>();
  std::unique_ptr<RestRouter> router_ = std::make_unique<RestRouter>(client_);
};

TEST_F(RestRouterSuite, ShouldSendBuyOrder) {
  StdoutCaptureGuard capture_log{};
  router_->SendLimit(kSym, Side::kBuy, kPrice, kQty);
  EXPECT_EQ(capture_log.Capture(),
            "REST: AAPL Buy 5 @ 100 for GTC -> REST-ORDER-ID\n");
}

class StrategySuite : public AdapterSuite {
 protected:
  std::shared_ptr<FixLib::Session> sess_ = std::make_shared<FixLib::Session>();
  std::unique_ptr<Strategy> strategy_ =
      std::make_unique<Strategy>(std::make_unique<FixRouter>(sess_));
};

TEST_F(StrategySuite, ShouldExecuteStrategyOrder) {
  StdoutCaptureGuard capture_log{};
  strategy_->Run();
  EXPECT_EQ(capture_log.Capture(), "FIX: AAPL B 100 @ 100 -> FIX-ORDER-ID\n");
}
