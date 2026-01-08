//
// Created by Will George on 1/6/26.
//

#include <iostream>

#include "chain_of_responsibility.h"

int main() {
  const auto kChain = std::make_shared<RiskHandler>(
      std::make_shared<ExecutionHandler>(nullptr));

  const TradeRequest kValidBuy{
      .ticker = "AAPL", .quantity = 100, .price = 15000};

  std::cout << "Processing valid trade...\n";
  absl::Status result = kChain->Operation(kValidBuy);
  std::cout << "Result: " << result << "\n\n";

  const TradeRequest kValidSell{
      .ticker = "AAPL", .quantity = -100, .price = 15000};

  std::cout << "Processing valid trade...\n";
  result = kChain->Operation(kValidSell);
  std::cout << "Result: " << result << "\n\n";

  const TradeRequest kInvalidTicker{
      .ticker = "TSLA", .quantity = 50, .price = 7500};

  std::cout << "Processing invalid ticker...\n";
  result = kChain->Operation(kInvalidTicker);
  std::cout << "Result: " << result << "\n\n";

  const TradeRequest kRiskyTrade{
      .ticker = "MSFT", .quantity = 1'000'000, .price = 1'000'000'00};

  std::cout << "Processing risky trade...\n";
  result = kChain->Operation(kRiskyTrade);
  std::cout << "Result: " << result << "\n";

  return 0;
}
