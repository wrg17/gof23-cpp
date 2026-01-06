//
// Created by Will George on 12/14/25.
//

#include <iostream>

#include "proxy.h"

int main() {
  auto stripe = std::make_unique<StripeGateway>("a fake key");

  PaymentGatewayProxy proxy{std::move(stripe), "payment-service", 100000};

  std::cout << "=== Proxy Pattern Demo: Payment Gateway ===\n\n";

  std::cout << "--- Payment 1: $250.00 ---\n";
  auto result1 = proxy.CreatePayment("token_visa_1234", 25000, '$',
                                     "Premium subscription");
  if (result1.ok()) {
    std::cout << "Payment String: " << *result1 << "\n\n";
  } else {
    std::cout << "Error: " << result1.status().message() << "\n\n";
  }

  std::cout << "--- Payment 2: $1000.00 (exceeds limit) ---\n";
  auto result4 =
      proxy.CreatePayment("token_amex_4321", 100000, '$', "Too much");
  if (result4.ok()) {
    std::cout << "Payment ID: " << *result4 << "\n\n";
  } else {
    std::cout << "Error: " << result4.status().message() << "\n\n";
  }

  std::cout << "=== Demo Complete ===\n";

  return 0;
}