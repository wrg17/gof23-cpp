//
// Created by Will George on 10/21/25.
//

#include <iostream>

#include "bridge.h"

int main() {
  EuropeanOption call{120, 100, 1, 0.01, 0.25, Right::kCall};

  call.SetEngine(std::make_shared<BlackScholesEngine>());
  std::cout << "BS   call ≈ " << call.Value() << "\n";

  call.SetEngine(std::make_shared<DummyEngine>());
  std::cout << "Fast call ≈ " << call.Value() << "\n";
}