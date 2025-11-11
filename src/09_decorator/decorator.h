//
// Created by Will George on 11/6/25.
//

#ifndef GOF23_DECORATOR_H
#define GOF23_DECORATOR_H

#include <array>
#include <cstddef>
#include <memory>
#include <string>

constexpr int64_t kPriceMultiplier = 10000;
constexpr std::size_t kMaxSymbolLength = 6;

using Symbol = std::array<char, kMaxSymbolLength>;

struct Order {
  Symbol symbol_{};
  std::int64_t price_{};
  std::int64_t quantity_{};
};

class IOrderService {
 public:
  virtual ~IOrderService();
  virtual void Execute(const Order& order) = 0;
};

class ExchangeOrderService final : public IOrderService {
 public:
  void Execute(const Order& order) override;
};

class OrderServiceDecorator : public IOrderService {
 public:
  explicit OrderServiceDecorator(std::unique_ptr<IOrderService> inner);

 protected:
  [[nodiscard]] IOrderService& Inner() const;

 private:
  std::unique_ptr<IOrderService> inner_;
};

class LoggingDecoratorService final : public OrderServiceDecorator {
 public:
  using OrderServiceDecorator::OrderServiceDecorator;

  void Execute(const Order& order) override;
};

#endif  // GOF23_DECORATOR_H
