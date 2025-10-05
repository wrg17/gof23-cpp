//
// Created by Will George on 9/23/25.
//

#include "abstract_factory.h"

#include <iostream>

namespace {
class XnasGw final : public OrderGateway {
 public:
  explicit XnasGw(OeParams params) : params_(std::move(params)) {};

  ~XnasGw() override = default;

  uint64_t LimitOrder(std::string_view symbol, double price, int32_t qty,
                      const Side kSide) override {
    const uint64_t kToken = ++token_;
    std::cout << std::format("[XNAS] {} {} {} @ {} via {}:{} -> token: {}\n",
                             GetSide(kSide), qty, symbol, price, params_.host_,
                             params_.port_, kToken);
    return kToken;
  };

  void Cancel(uint64_t token) override {
    std::cout << std::format("[XNAS] Cancel token: {}\n", token);
  };

 private:
  OeParams params_;
  uint64_t token_{0};
};

class CmeGw final : public OrderGateway {
 public:
  explicit CmeGw(OeParams params) : params_(std::move(params)) {};

  ~CmeGw() override = default;
  uint64_t LimitOrder(std::string_view symbol, double price, int32_t qty,
                      const Side kSide) override {
    const uint64_t kToken = ++token_;
    std::cout << std::format("[CME ] {} {} {} @ {} via {}:{} -> token: {}\n",
                             GetSide(kSide), qty, symbol, price, params_.host_,
                             params_.port_, kToken);
    return kToken;
  };

  void Cancel(uint64_t token) override {
    std::cout << std::format("[CME ] Cancel token: {}\n", token);
  };

 private:
  OeParams params_;
  uint64_t token_{0};
};

class XnasFactory final : public VenueFactory {
  std::unique_ptr<OrderGateway> MakeOrderGateway(
      const OeParams& param) override {
    return std::make_unique<XnasGw>(param);
  }
};

class CmeFactory final : public VenueFactory {
  std::unique_ptr<OrderGateway> MakeOrderGateway(
      const OeParams& param) override {
    return std::make_unique<CmeGw>(param);
  }
};
}  // namespace

std::unique_ptr<VenueFactory> MakeVenueFactory(const Venue kVenue) {
  switch (kVenue) {
    case Venue::kXNAS:
      return std::make_unique<XnasFactory>();
    case Venue::kCME:
      return std::make_unique<CmeFactory>();
    default:
      return nullptr;
  }
}

const std::type_info& FactoryImplOf(const Venue kVenue) noexcept {
  switch (kVenue) {
    case Venue::kXNAS:
      return typeid(XnasFactory);
    case Venue::kCME:
      return typeid(CmeFactory);
  }
  return typeid(void);
}

bool IsFactoryImpl(const VenueFactory& factory, const Venue kVenue) noexcept {
  return typeid(factory) == FactoryImplOf(kVenue);
}

const std::type_info& GatewayImplOf(const Venue kVenue) noexcept {
  switch (kVenue) {
    case Venue::kXNAS:
      return typeid(XnasGw);
    case Venue::kCME:
      return typeid(CmeGw);
  }
  return typeid(void);
}

bool IsGatewayImpl(const OrderGateway& gateway, const Venue kVenue) noexcept {
  return typeid(gateway) == GatewayImplOf(kVenue);
}