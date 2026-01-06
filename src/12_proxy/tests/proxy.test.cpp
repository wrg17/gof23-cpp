//
// Created by Will George on 12/14/25.
//

#include "proxy.h"

#include <memory>

#include <gtest/gtest.h>

#include "../helpers/StdoutCaptureGuard.h"

class MockPaymentGateway final : public IPaymentGateway {
 public:
  explicit MockPaymentGateway(const bool kShouldSucceed = true)
      : should_succeed_(kShouldSucceed) {}

  absl::StatusOr<std::string> CreatePayment(
      const std::string& processor_token, const std::int64_t amount_cents,
      const char currency, const std::string& description) override {
    call_count_++;
    last_token_ = processor_token;
    last_amount_ = amount_cents;
    last_currency_ = currency;
    last_description_ = description;

    if (should_succeed_) {
      return std::format("mock-payment-{}", processor_token);
    }
    return absl::InternalError("payment processing failed");
  }

  int call_count_{0};
  std::string last_token_;
  std::int64_t last_amount_{0};
  char last_currency_{'\0'};
  std::string last_description_;
  bool should_succeed_;
};

class StripeGatewaySuite : public ::testing::Test {
 protected:
  static constexpr std::string kApiKey = "stripe_key_example";
  static constexpr std::string kToken = "token_visa";
  static constexpr std::int64_t kAmount = 10050;
  static constexpr char kCurrency = '$';
  static constexpr std::string kDescription = "Test payment";
};

TEST_F(StripeGatewaySuite, shouldCreatePayment) {
  StripeGateway gateway{kApiKey};
  StdoutCaptureGuard capture{};

  const auto kResult =
      gateway.CreatePayment(kToken, kAmount, kCurrency, kDescription);

  ASSERT_TRUE(kResult.ok());
  EXPECT_EQ(*kResult, "stripe-payment-token_visa");

  const auto kOutput = capture.Capture();
  EXPECT_TRUE(kOutput.find("Stripe: processing payment") != std::string::npos);
  EXPECT_TRUE(kOutput.find("token_visa") != std::string::npos);
  EXPECT_TRUE(kOutput.find("$100.50") != std::string::npos);
  EXPECT_TRUE(kOutput.find("Test payment") != std::string::npos);
  EXPECT_TRUE(kOutput.find(kApiKey) != std::string::npos);
}

TEST_F(StripeGatewaySuite, shouldHandleNegativeAmounts) {
  StripeGateway gateway{kApiKey};
  StdoutCaptureGuard capture{};

  constexpr std::int64_t kNegativeAmount = -5025;
  const auto result =
      gateway.CreatePayment(kToken, kNegativeAmount, kCurrency, kDescription);

  ASSERT_TRUE(result.ok());
  const auto output = capture.Capture();
  EXPECT_TRUE(output.find("$-50.25") != std::string::npos);
}

class PaymentGatewayProxySuite : public ::testing::Test {
 protected:
  static constexpr std::string kServiceName = "test-service";
  static constexpr std::int64_t kDailyLimit = 50000;
  static constexpr std::string kToken = "token_test";
  static constexpr char kCurrency = '$';
  static constexpr std::string kDescription = "Test transaction";

  static std::unique_ptr<MockPaymentGateway> CreateMockGateway(
      bool should_succeed = true) {
    return std::make_unique<MockPaymentGateway>(should_succeed);
  }
};

TEST_F(PaymentGatewayProxySuite, shouldDelegateToRealGateway) {
  auto mock = CreateMockGateway();
  auto* mock_ptr = mock.get();

  PaymentGatewayProxy proxy{std::move(mock), kServiceName, kDailyLimit};
  StdoutCaptureGuard capture{};

  constexpr std::int64_t kAmount = 10000;
  const auto result =
      proxy.CreatePayment(kToken, kAmount, kCurrency, kDescription);

  ASSERT_TRUE(result.ok());
  EXPECT_EQ(*result, "mock-payment-token_test");
  EXPECT_EQ(mock_ptr->call_count_, 1);
  EXPECT_EQ(mock_ptr->last_token_, kToken);
  EXPECT_EQ(mock_ptr->last_amount_, kAmount);
  EXPECT_EQ(mock_ptr->last_currency_, kCurrency);
  EXPECT_EQ(mock_ptr->last_description_, kDescription);
}

TEST_F(PaymentGatewayProxySuite, shouldLogTransaction) {
  auto mock = CreateMockGateway();
  PaymentGatewayProxy proxy{std::move(mock), kServiceName, kDailyLimit};

  StdoutCaptureGuard capture{};
  constexpr std::int64_t kAmount = 12345;

  auto payment = proxy.CreatePayment(kToken, kAmount, kCurrency, kDescription);
  ASSERT_TRUE(payment.ok());

  const auto output = capture.Capture();
  EXPECT_TRUE(output.find("test-service:") != std::string::npos);
  EXPECT_TRUE(output.find("payment") != std::string::npos);
  EXPECT_TRUE(output.find("$123.45") != std::string::npos);
  EXPECT_TRUE(output.find("Test transaction") != std::string::npos);
}

TEST_F(PaymentGatewayProxySuite, shouldEnforceDailyLimit) {
  auto mock = CreateMockGateway();
  PaymentGatewayProxy proxy{std::move(mock), kServiceName, kDailyLimit};

  constexpr std::int64_t kExcessiveAmount = 60000;
  const auto result =
      proxy.CreatePayment(kToken, kExcessiveAmount, kCurrency, kDescription);

  ASSERT_FALSE(result.ok());
  EXPECT_EQ(result.status().code(), absl::StatusCode::kResourceExhausted);
  EXPECT_TRUE(result.status().message().find("daily limit exceeded") !=
              std::string::npos);
}

TEST_F(PaymentGatewayProxySuite, shouldAccumulateDailyTotal) {
  auto mock = CreateMockGateway();
  auto* mock_ptr = mock.get();

  PaymentGatewayProxy proxy{std::move(mock), kServiceName, kDailyLimit};

  constexpr std::int64_t kFirstAmount = 20000;
  auto result1 =
      proxy.CreatePayment(kToken, kFirstAmount, kCurrency, kDescription);
  ASSERT_TRUE(result1.ok());

  constexpr std::int64_t kSecondAmount = 20000;
  auto result2 =
      proxy.CreatePayment(kToken, kSecondAmount, kCurrency, kDescription);
  ASSERT_TRUE(result2.ok());

  constexpr std::int64_t kThirdAmount = 20000;
  auto result3 =
      proxy.CreatePayment(kToken, kThirdAmount, kCurrency, kDescription);
  ASSERT_FALSE(result3.ok());
  EXPECT_EQ(result3.status().code(), absl::StatusCode::kResourceExhausted);

  EXPECT_EQ(mock_ptr->call_count_, 2);
}

TEST_F(PaymentGatewayProxySuite, shouldAllowPaymentAtExactLimit) {
  auto mock = CreateMockGateway();
  PaymentGatewayProxy proxy{std::move(mock), kServiceName, kDailyLimit};

  const auto kResult =
      proxy.CreatePayment(kToken, kDailyLimit, kCurrency, kDescription);

  ASSERT_TRUE(kResult.ok());
  EXPECT_EQ(*kResult, "mock-payment-token_test");
}

TEST_F(PaymentGatewayProxySuite, shouldHandleMultipleSmallPayments) {
  auto mock = CreateMockGateway();
  auto* mock_ptr = mock.get();

  PaymentGatewayProxy proxy{std::move(mock), kServiceName, kDailyLimit};

  constexpr std::int64_t kSmallAmount = 5000;
  constexpr int kNumPayments = 10;

  for (int i = 0; i < kNumPayments; ++i) {
    auto result =
        proxy.CreatePayment(kToken, kSmallAmount, kCurrency, kDescription);
    ASSERT_TRUE(result.ok()) << "Payment " << i << " failed";
  }

  EXPECT_EQ(mock_ptr->call_count_, kNumPayments);

  auto result =
      proxy.CreatePayment(kToken, kSmallAmount, kCurrency, kDescription);
  ASSERT_FALSE(result.ok());
  EXPECT_EQ(result.status().code(), absl::StatusCode::kResourceExhausted);
}

TEST_F(PaymentGatewayProxySuite, shouldHandleZeroAmount) {
  auto mock = CreateMockGateway();
  PaymentGatewayProxy proxy{std::move(mock), kServiceName, kDailyLimit};

  constexpr std::int64_t kZeroAmount = 0;
  const auto kResult =
      proxy.CreatePayment(kToken, kZeroAmount, kCurrency, kDescription);

  ASSERT_TRUE(kResult.ok());
}