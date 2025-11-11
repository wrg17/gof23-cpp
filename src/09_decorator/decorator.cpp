//
// Created by Will George on 11/6/25.
//

#include "decorator.h"

#include <format>
#include <iostream>

#include <absl/strings/str_format.h>

IOrderService::~IOrderService() = default;

void ExchangeOrderService::Execute(const Order& order) {
  const double kPriceDouble =
      static_cast<double>(order.price_) / kPriceMultiplier;

  std::cout << std::format("[ORDER] {: <5}: ${:.4f} x {}\n",
                           std::string_view(order.symbol_.data()), kPriceDouble,
                           order.quantity_);
}

OrderServiceDecorator::OrderServiceDecorator(
    std::unique_ptr<IOrderService> inner)
    : inner_(std::move(inner)) {}

IOrderService& OrderServiceDecorator::Inner() const { return *inner_; }

void LoggingDecoratorService::Execute(const Order& order) {
  std::cout << "[LOG  ] Placing order\n";
  Inner().Execute(order);
  std::cout << "[LOG  ] Completed order\n";
}
