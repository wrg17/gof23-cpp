//
// Created by Will George on 9/23/25.
//

#include "abstract_factory.h"

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

#include <gtest/gtest.h>

#include "../helpers/StdoutCaptureGuard.h"

class MakeVenueFactorySuite : public ::testing::Test {};

TEST_F(MakeVenueFactorySuite, MakeCmeFactory) {
  const auto kFactory = MakeVenueFactory(Venue::kCME);
  ASSERT_TRUE(IsFactoryImpl(*kFactory, Venue::kCME));
}

TEST_F(MakeVenueFactorySuite, MakeXnasFactory) {
  const auto kFactory = MakeVenueFactory(Venue::kXNAS);
  ASSERT_TRUE(IsFactoryImpl(*kFactory, Venue::kXNAS));
}

class MakeGatewaySuite : public ::testing::Test {
 public:
  static constexpr std::string kHost = "localhost";
  static constexpr uint16_t kPort = 9000;
  static constexpr std::string kUser = "trader";
  static constexpr std::string kPassword = "a secure password";
  static constexpr OeParams kBasicGwArgs{
      .host_ = kHost, .port_ = kPort, .user_ = kUser, .pass_ = kPassword};
};

TEST_F(MakeGatewaySuite, MakeCmeGateway) {
  const auto kFactory = MakeVenueFactory(Venue::kCME);
  ASSERT_TRUE(IsFactoryImpl(*kFactory, Venue::kCME));

  const auto kGateway = kFactory->MakeOrderGateway(kBasicGwArgs);
  ASSERT_TRUE(IsGatewayImpl(*kGateway, Venue::kCME));
}

TEST_F(MakeGatewaySuite, MakeXnasGateway) {
  const auto kFactory = MakeVenueFactory(Venue::kXNAS);
  ASSERT_TRUE(IsFactoryImpl(*kFactory, Venue::kXNAS));

  const auto kGateway = kFactory->MakeOrderGateway(kBasicGwArgs);
  ASSERT_TRUE(IsGatewayImpl(*kGateway, Venue::kXNAS));
}

class OrderManagementSuite : public MakeGatewaySuite {
 public:
  static constexpr std::string kTicker = "JPM";
  static constexpr double kPrice = 310.03;
  static constexpr uint32_t kQty = 100;
};

TEST_F(OrderManagementSuite, AddAndCancelCmeBuyOrders) {
  const auto kFactory = MakeVenueFactory(Venue::kCME);
  ASSERT_TRUE(IsFactoryImpl(*kFactory, Venue::kCME));

  const auto kGateway = kFactory->MakeOrderGateway(kBasicGwArgs);
  ASSERT_TRUE(IsGatewayImpl(*kGateway, Venue::kCME));

  StdoutCaptureGuard capture_log{};
  const uint64_t kToken =
      kGateway->LimitOrder(kTicker, kPrice, kQty, Side::kBuy);

  EXPECT_EQ(capture_log.Capture(),
            "[CME ] Buy 100 JPM @ 310.03 via localhost:9000 -> token: 1\n");
  EXPECT_EQ(kToken, 1);

  capture_log.Resume();
  kGateway->Cancel(kToken);

  EXPECT_EQ(capture_log.Capture(), "[CME ] Cancel token: 1\n");
}

TEST_F(OrderManagementSuite, AddAndCancelXnasBuyOrders) {
  const auto kFactory = MakeVenueFactory(Venue::kXNAS);
  ASSERT_TRUE(IsFactoryImpl(*kFactory, Venue::kXNAS));

  const auto kGateway = kFactory->MakeOrderGateway(kBasicGwArgs);
  ASSERT_TRUE(IsGatewayImpl(*kGateway, Venue::kXNAS));

  StdoutCaptureGuard capture_log{};
  const uint64_t kToken =
      kGateway->LimitOrder(kTicker, kPrice, kQty, Side::kBuy);

  EXPECT_EQ(capture_log.Capture(),
            "[XNAS] Buy 100 JPM @ 310.03 via localhost:9000 -> token: 1\n");

  EXPECT_EQ(kToken, 1);

  capture_log.Resume();
  kGateway->Cancel(kToken);

  EXPECT_EQ(capture_log.Capture(), "[XNAS] Cancel token: 1\n");
}

TEST_F(OrderManagementSuite, AddAndCancelCmeSellOrders) {
  const auto kFactory = MakeVenueFactory(Venue::kCME);
  ASSERT_TRUE(IsFactoryImpl(*kFactory, Venue::kCME));

  const auto kGateway = kFactory->MakeOrderGateway(kBasicGwArgs);
  ASSERT_TRUE(IsGatewayImpl(*kGateway, Venue::kCME));

  StdoutCaptureGuard capture_log{};
  const uint64_t kToken =
      kGateway->LimitOrder(kTicker, kPrice, kQty, Side::kSell);

  EXPECT_EQ(capture_log.Capture(),
            "[CME ] Sell 100 JPM @ 310.03 via localhost:9000 -> token: 1\n");
  EXPECT_EQ(kToken, 1);

  capture_log.Resume();
  kGateway->Cancel(kToken);

  EXPECT_EQ(capture_log.Capture(), "[CME ] Cancel token: 1\n");
}

TEST_F(OrderManagementSuite, AddAndCancelXnasSellOrders) {
  const auto kFactory = MakeVenueFactory(Venue::kXNAS);
  ASSERT_TRUE(IsFactoryImpl(*kFactory, Venue::kXNAS));

  const auto kGateway = kFactory->MakeOrderGateway(kBasicGwArgs);
  ASSERT_TRUE(IsGatewayImpl(*kGateway, Venue::kXNAS));

  StdoutCaptureGuard capture_log{};

  const uint64_t kToken =
      kGateway->LimitOrder(kTicker, kPrice, kQty, Side::kSell);

  EXPECT_EQ(capture_log.Capture(),
            "[XNAS] Sell 100 JPM @ 310.03 via localhost:9000 -> token: 1\n");
  EXPECT_EQ(kToken, 1);

  capture_log.Resume();
  kGateway->Cancel(kToken);

  EXPECT_EQ(capture_log.Capture(), "[XNAS] Cancel token: 1\n");
}

TEST_F(OrderManagementSuite, AddCmeBuyAndSellOrders) {
  const auto kFactory = MakeVenueFactory(Venue::kCME);
  ASSERT_TRUE(IsFactoryImpl(*kFactory, Venue::kCME));

  const auto kGateway = kFactory->MakeOrderGateway(kBasicGwArgs);
  ASSERT_TRUE(IsGatewayImpl(*kGateway, Venue::kCME));

  StdoutCaptureGuard capture_log{};

  uint64_t token = kGateway->LimitOrder(kTicker, kPrice, kQty, Side::kBuy);
  EXPECT_EQ(capture_log.Capture(),
            "[CME ] Buy 100 JPM @ 310.03 via localhost:9000 -> token: 1\n");
  EXPECT_EQ(token, 1);

  capture_log.Resume();
  token = kGateway->LimitOrder(kTicker, kPrice + 1, kQty / 2, Side::kSell);

  EXPECT_EQ(capture_log.Capture(),
            "[CME ] Sell 50 JPM @ 311.03 via localhost:9000 -> token: 2\n");

  EXPECT_EQ(token, 2);
}

TEST_F(OrderManagementSuite, AddXnasBuyAndSellOrders) {
  const auto kFactory = MakeVenueFactory(Venue::kXNAS);
  ASSERT_TRUE(IsFactoryImpl(*kFactory, Venue::kXNAS));

  const auto kGateway = kFactory->MakeOrderGateway(kBasicGwArgs);
  ASSERT_TRUE(IsGatewayImpl(*kGateway, Venue::kXNAS));

  StdoutCaptureGuard capture_log{};

  uint64_t token = kGateway->LimitOrder(kTicker, kPrice, kQty, Side::kBuy);

  EXPECT_EQ(capture_log.Capture(),
            "[XNAS] Buy 100 JPM @ 310.03 via localhost:9000 -> token: 1\n");

  EXPECT_EQ(token, 1);

  capture_log.Resume();
  token = kGateway->LimitOrder(kTicker, kPrice + 1, kQty / 2, Side::kSell);

  EXPECT_EQ(capture_log.Capture(),
            "[XNAS] Sell 50 JPM @ 311.03 via localhost:9000 -> token: 2\n");

  EXPECT_EQ(token, 2);
}