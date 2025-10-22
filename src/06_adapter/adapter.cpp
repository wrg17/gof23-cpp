//
// Created by Will George on 10/20/25.
//

#include "adapter.h"

#include <iostream>
#include <memory>
#include <string>

namespace FixLib {
OrderId Session::NewOrderSingle(const std::string& sym, char fixSide,
                                uint32_t qty, double price) {
  std::cout << std::format("FIX: {} {} {} @ {} -> {}\n", sym, fixSide, qty,
                           price, "FIX-ORDER-ID");
  return "FIX-ORDER-ID";
}
}  // namespace FixLib

namespace RestLib {
OrderId Client::PostOrder(const std::string& sym, const std::string& side,
                          uint32_t qty, double price,
                          const std::string& timeInForce) {
  std::cout << std::format("REST: {} {} {} @ {} for {} -> {}\n", sym, side, qty,
                           price, timeInForce, "REST-ORDER-ID");
  return "REST-ORDER-ID";
}
}  // namespace RestLib

FixRouter::FixRouter(std::shared_ptr<FixLib::Session> sess)
    : sess_(std::move(sess)) {};

OrderId FixRouter::SendLimit(const std::string& sym, const Side side,
                             const double price, const uint32_t qty) {
  const char fixSide = side == Side::kBuy ? 'B' : 'S';

  return sess_->NewOrderSingle(sym, fixSide, qty, price);
};

RestRouter::RestRouter(std::shared_ptr<RestLib::Client> cli)
    : cli_(std::move(cli)) {};
OrderId RestRouter::SendLimit(const std::string& sym, const Side side,
                              const double price, const uint32_t qty) {
  const std::string kSide = side == Side::kBuy ? "Buy" : "Sell";
  return cli_->PostOrder(sym, kSide, qty, price, "GTC");
};

Strategy::Strategy(std::unique_ptr<IOrderRouter> router)
    : router_(std::move(router)) {};

void Strategy::Run() const {
  router_->SendLimit("AAPL", Side::kBuy, 100, 100);
};
