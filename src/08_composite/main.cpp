//
// Created by Will George on 10/24/25.
//

#include "composite.h"

int main() {
  auto book_or_status = Portfolio::Create();
  if (!book_or_status.ok()) return 1;
  const auto book = std::move(book_or_status).value();

  auto stock_or_status = Stock::Create(Ticker{"AAPL", "apple"}, 50, 100.00);
  if (!stock_or_status.ok()) return 1;

  auto stock = std::move(stock_or_status).value();
  if (const auto status = book->Add(std::move(stock)); !status.ok()) return 1;

  std::cout << book->NetCost() << "\n";
  std::cout << book->ToString() << "\n";
  return 0;
}