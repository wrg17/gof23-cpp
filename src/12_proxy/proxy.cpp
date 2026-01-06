//
// Created by Will George on 12/14/25.
//

#include "proxy.h"

#include <cstdlib>
#include <format>
#include <iostream>

StripeGateway::StripeGateway(std::string api_key)
    : api_key_(std::move(api_key)) {}

absl::StatusOr<std::string> StripeGateway::CreatePayment(
    const std::string& processor_token, const std::int64_t amount_cents,
    const char currency, const std::string& description) {
  auto dollars = amount_cents / 100;
  auto cents = std::abs(amount_cents % 100);

  std::cout << std::format(
      "Stripe: processing payment {} {}{}.{:02} <- \"{}\" for {}\n",
      processor_token, currency, dollars, cents, description, api_key_);

  return std::format("stripe-payment-{}", processor_token);
}

PaymentGatewayProxy::PaymentGatewayProxy(
    std::unique_ptr<IPaymentGateway> real_gateway, std::string service_name,
    std::int64_t daily_limit_cents)
    : real_gateway_(std::move(real_gateway)),
      service_name_(std::move(service_name)),
      daily_limit_cents_(daily_limit_cents) {}

absl::StatusOr<std::string> PaymentGatewayProxy::CreatePayment(
    const std::string& processor_token, const std::int64_t amount_cents,
    const char currency, const std::string& description) {
  if (const bool kIsInLimit = WithinDailyLimit(amount_cents); !kIsInLimit) {
    return absl::ResourceExhaustedError("daily limit exceeded");
  }
  LogTransaction("payment", amount_cents, currency, description);

  auto result = real_gateway_->CreatePayment(processor_token, amount_cents,
                                             currency, description);
  if (result.ok()) {
    daily_total_cents_ += amount_cents;
  }

  return result;
}

void PaymentGatewayProxy::LogTransaction(const std::string& type,
                                         const std::int64_t amount,
                                         const char currency,
                                         const std::string& description) const {
  auto dollars = amount / 100;
  auto cents = std::abs(amount % 100);

  std::cout << std::format("{}: {} {}{}.{:02} <- \"{}\"\n", service_name_, type,
                           currency, dollars, cents, description);
}

bool PaymentGatewayProxy::WithinDailyLimit(
    const std::int64_t amount_cents) const {
  return daily_limit_cents_ >= daily_total_cents_ + amount_cents;
}