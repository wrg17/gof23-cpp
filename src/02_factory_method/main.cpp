//
// Created by Will George on 9/16/25.
//

#include <iostream>
#include <vector>

#include "factory_method.h"

int main() {
  std::vector<std::unique_ptr<Bakery> > shops;
  shops.push_back(std::make_unique<FrenchBakery>("French Bakery", "Pierre"));
  shops.push_back(std::make_unique<DonutShop>("Donut Shop", "Will"));

  for (const auto& bakery : shops) {
    const auto kCroissant = bakery->Fulfill("chocolate", true);
    std::cout << kCroissant->Describe() << "\n\n";

    const auto kDonut = bakery->Fulfill("pistachio", false);
    std::cout << kDonut->Describe() << "\n\n";
  }
}