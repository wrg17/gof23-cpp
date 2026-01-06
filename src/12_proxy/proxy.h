//
// Created by Will George on 12/14/25.
//

#ifndef GOF23_PROXY_H
#define GOF23_PROXY_H

#include <absl/status/statusor.h>

class IPaymentGateway {
 public:
  virtual ~IPaymentGateway() = default;

  virtual absl::StatusOr<std::string> CreatePayment(
      const std::string& processor_token, std::int64_t amount_cents,
      char currency, const std::string& description) = 0;
};

class StripeGateway final : public IPaymentGateway {
 public:
  explicit StripeGateway(std::string api_key);

  absl::StatusOr<std::string> CreatePayment(
      const std::string& processor_token, std::int64_t amount_cents,
      char currency, const std::string& description) override;

 private:
  std::string api_key_;
};

class PaymentGatewayProxy final : public IPaymentGateway {
 public:
  explicit PaymentGatewayProxy(std::unique_ptr<IPaymentGateway> real_gateway,
                               std::string service_name,
                               std::int64_t daily_limit_cents);

  absl::StatusOr<std::string> CreatePayment(
      const std::string& processor_token, std::int64_t amount_cents,
      char currency, const std::string& description) override;

 private:
  void LogTransaction(const std::string& type, std::int64_t amount,
                      char currency, const std::string& description) const;

  bool WithinDailyLimit(std::int64_t amount_cents) const;

  std::unique_ptr<IPaymentGateway> real_gateway_;
  std::string service_name_;

  std::int64_t daily_limit_cents_;
  std::int64_t daily_total_cents_{0};
};

#endif  // GOF23_PROXY_H
