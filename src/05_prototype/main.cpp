//
// Created by Will George on 10/8/25.
//

#include "prototype.h"

int main() {
  auto heavy = std::make_shared<ParserCfg>();
  auto proto = std::make_unique<ConcreteParser>(heavy);

  const ParserFactory kFactory(std::move(proto),
                               SessionCfg{.id_ = "Alpha", .tickers_ = {}});

  const auto kParserA =
      kFactory.Make({.id_ = std::string("Alpha"),
                     .tickers_ = std::vector<std::string>{"AAPL", "MSFT"}});
  const auto kParserB =
      kFactory.Make({.id_ = std::string("Beta"), .tickers_ = {}});

  kParserA->Parse("AAPL");
  kParserA->Parse("GOOG");
  kParserB->Parse("GOOG");
}