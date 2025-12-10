//
// Created by Will George on 12/8/25.
//

#include "flyweight.h"

#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <gtest/gtest.h>

#include "../helpers/StdoutCaptureGuard.h"

class FlyweightSuite : public ::testing::Test {
 protected:
  static constexpr absl::string_view kUSD = "USD";
  static constexpr absl::string_view kEUR = "EUR";
  static constexpr absl::string_view kGBP = "GBP";
  static constexpr absl::string_view kJPY = "JPY";
  static constexpr uint8_t kStandardDecimals = 2;
  static constexpr uint8_t kJpyDecimals = 0;
  static constexpr absl::string_view kDollarSymbol = "$";
  static constexpr absl::string_view kEuroSymbol = "€";
  static constexpr absl::string_view kPoundSymbol = "£";
  static constexpr absl::string_view kYenSymbol = "¥";
  static constexpr std::int64_t kAmount1 = 12345;
  static constexpr std::int64_t kAmount2 = 45678;
  static constexpr std::uint64_t kTimestamp = 1702000000;
};

TEST_F(FlyweightSuite, Currency_USD) {
  const Currency kUsd(kUSD, kStandardDecimals, kDollarSymbol);

  EXPECT_EQ(kUsd.GetCode(), kUSD);
  EXPECT_EQ(kUsd.GetDecimalPlaces(), kStandardDecimals);
  EXPECT_EQ(kUsd.GetSymbol(), kDollarSymbol);
}

TEST_F(FlyweightSuite, Currency_JPY) {
  const Currency kJpy(kJPY, kJpyDecimals, kYenSymbol);

  EXPECT_EQ(kJpy.GetCode(), kJPY);
  EXPECT_EQ(kJpy.GetDecimalPlaces(), kJpyDecimals);
  EXPECT_EQ(kJpy.GetSymbol(), kYenSymbol);
}

TEST_F(FlyweightSuite, CurrencyFactory_GetCurrency_CreatesCurrencyAndLog) {
  CurrencyFactory factory;

  StdoutCaptureGuard guard;
  auto status_or_currency =
      factory.GetCurrency(kUSD, kStandardDecimals, kDollarSymbol);
  const std::string kOut = guard.Capture();

  ASSERT_TRUE(status_or_currency.ok());

  const auto kCurrency = std::move(status_or_currency).value();
  EXPECT_EQ(kCurrency->GetCode(), kUSD);
  EXPECT_EQ(kCurrency->GetDecimalPlaces(), kStandardDecimals);
  EXPECT_EQ(kCurrency->GetSymbol(), kDollarSymbol);

  EXPECT_NE(kOut.find("New currency created: USD"), std::string::npos);
  EXPECT_EQ(kOut.find('\0'), std::string::npos);
}

TEST_F(FlyweightSuite,
       CurrencyFactory_GetCurrency_GetCachedCurrencyOnSecondCall) {
  CurrencyFactory factory;

  auto currency1 = factory.GetCurrency(kUSD, kStandardDecimals, kDollarSymbol);
  ASSERT_TRUE(currency1.ok());

  auto currency2 = factory.GetCurrency(kUSD, kStandardDecimals, kDollarSymbol);
  ASSERT_TRUE(currency2.ok());

  EXPECT_EQ(currency1.value().get(), currency2.value().get());
}

TEST_F(FlyweightSuite, CurrencyFactory_GetCurrency_CreatesDistinctCurrencies) {
  CurrencyFactory factory;

  auto usd = factory.GetCurrency(kUSD, kStandardDecimals, kDollarSymbol);
  ASSERT_TRUE(usd.ok());

  auto eur = factory.GetCurrency(kEUR, kStandardDecimals, kEuroSymbol);
  ASSERT_TRUE(eur.ok());

  EXPECT_NE(usd.value().get(), eur.value().get());
}

TEST_F(FlyweightSuite, CurrencyFactory_GetCurrency_ReturnsCachedCurrency) {
  CurrencyFactory factory;

  auto create_result =
      factory.GetCurrency(kUSD, kStandardDecimals, kDollarSymbol);
  ASSERT_TRUE(create_result.ok());

  auto get_result = factory.GetCurrency(kUSD);
  ASSERT_TRUE(get_result.ok());

  EXPECT_EQ(create_result.value().get(), get_result.value().get());
}

TEST_F(FlyweightSuite, CurrencyFactory_GetCurrency_FailsForUnknownCurrency) {
  const CurrencyFactory kFactory;

  const auto kResult = kFactory.GetCurrency("UNKNOWN");

  EXPECT_FALSE(kResult.ok());
  EXPECT_EQ(kResult.status().code(), absl::StatusCode::kNotFound);
}

TEST_F(FlyweightSuite, CurrencyFactory_GetCurrency_CountsCreatedCurrencies) {
  CurrencyFactory factory;

  EXPECT_EQ(factory.GetCachedCurrencyCount(), 0U);

  auto result = factory.GetCurrency(kUSD, kStandardDecimals, kDollarSymbol);
  ASSERT_TRUE(result.ok());
  EXPECT_EQ(factory.GetCachedCurrencyCount(), 1U);

  result = factory.GetCurrency(kEUR, kStandardDecimals, kEuroSymbol);
  ASSERT_TRUE(result.ok());
  EXPECT_EQ(factory.GetCachedCurrencyCount(), 2U);

  result = factory.GetCurrency(kUSD, kStandardDecimals, kDollarSymbol);
  ASSERT_TRUE(result.ok());
  EXPECT_EQ(factory.GetCachedCurrencyCount(), 2U);
}

TEST_F(FlyweightSuite, Payment_Construct) {
  CurrencyFactory factory;
  auto currency = factory.GetCurrency(kUSD, kStandardDecimals, kDollarSymbol);
  ASSERT_TRUE(currency.ok());

  const Payment kPayment(currency.value(), kAmount1, kTimestamp);

  EXPECT_EQ(kPayment.currency_.get(), currency.value().get());
  EXPECT_EQ(kPayment.amount_, kAmount1);
  EXPECT_EQ(kPayment.timestamp_, kTimestamp);
}

TEST_F(FlyweightSuite, FlyweightPattern_MultiplePayments_ShareCurrencyObjects) {
  CurrencyFactory factory;

  auto usd = factory.GetCurrency(kUSD, kStandardDecimals, kDollarSymbol);
  auto eur = factory.GetCurrency(kEUR, kStandardDecimals, kEuroSymbol);
  ASSERT_TRUE(usd.ok());
  ASSERT_TRUE(eur.ok());

  auto mutable_timestamp = kTimestamp;

  const Payment kPayment1(usd.value(), kAmount1, mutable_timestamp);
  const Payment kPayment2(usd.value(), kAmount2, ++mutable_timestamp);
  const Payment kPayment3(eur.value(), kAmount1, ++mutable_timestamp);

  EXPECT_EQ(kPayment1.currency_.get(), kPayment2.currency_.get());
  EXPECT_NE(kPayment1.currency_.get(), kPayment3.currency_.get());

  EXPECT_EQ(factory.GetCachedCurrencyCount(), 2U);
}