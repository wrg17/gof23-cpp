//
// Created by Will George on 11/6/25.
//

#include "decorator.h"

int main() {
  constexpr Order order{
      .symbol_ = {"AAPL"}, .price_ = 2684700, .quantity_ = 100};

  std::unique_ptr<IOrderService> service = std::make_unique<ExchangeOrderService>();

  service = std::make_unique<LoggingDecoratorService>(std::move(service));

  service->Execute(order);

  return 0;
}