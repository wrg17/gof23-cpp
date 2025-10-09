//
// Created by Will George on 10/4/25.
//

#include <iostream>

#include "builder.h"

int main() {
  SimpleStrategyBuilder builder;
  const StrategyDirector kDirector{builder};

  const auto kIronCondor = kDirector.IronCondor(
      "SPY", "2025-12-19", 480, 2.00, 490, 5.20, 510, 5.10, 520, 2.10);

  std::cout << kIronCondor.name_ << " legs: " << kIronCondor.legs_.size()
            << "\n";
  return 0;
}