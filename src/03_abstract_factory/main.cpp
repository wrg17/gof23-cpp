//
// Created by Will George on 9/23/25.
//

#include "abstract_factory.h"

int main() {
  const auto kFactory = MakeVenueFactory(Venue::kCME);
  if (!kFactory) return 1;

  constexpr std::string kHost = "localhost";
  constexpr uint16_t kPort = 9000;
  constexpr std::string kUser = "trader";
  constexpr std::string kPassword = "a secure password";

  const OeParams kGatewayArgs{
      .host_ = kHost, .port_ = kPort, .user_ = kUser, .pass_ = kPassword};

  const auto kGateway = kFactory->MakeOrderGateway(kGatewayArgs);

  constexpr std::string kTicker = "JPM";
  constexpr double kPrice = 310.03;
  constexpr uint32_t kQty = 100;

  const auto kToken = kGateway->LimitOrder(kTicker, kPrice, kQty, Side::kBuy);
  kGateway->Cancel(kToken);

  return 0;
}