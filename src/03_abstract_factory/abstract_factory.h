//
// Created by Will George on 9/23/25.
//

#ifndef GOF23_ABSTRACT_FACTORY_H
#define GOF23_ABSTRACT_FACTORY_H

#include <memory>
#include <string_view>
#include <typeindex>
#include <vector>

enum class Venue : std::uint8_t { kXNAS, kCME };
enum class Side : std::uint8_t { kSell = 0, kBuy = 1 };

constexpr std::string_view GetSide(const Side kSide) {
  return kSide == Side::kBuy ? "Buy" : "Sell";
}

struct OeParams {
  std::string host_;
  uint16_t port_{};
  std::string user_;
  std::string pass_;
};

struct OrderGateway {
  virtual ~OrderGateway() = default;

  virtual uint64_t LimitOrder(
      std::string_view symbol, double price, int32_t qty,
      Side kSide) = 0;  // NOLINT(readability-identifier-naming)
  virtual void Cancel(uint64_t token) = 0;
};

struct VenueFactory {
  virtual ~VenueFactory() = default;
  virtual std::unique_ptr<OrderGateway> MakeOrderGateway(
      const OeParams& params) = 0;
};

std::unique_ptr<VenueFactory> MakeVenueFactory(
    Venue kVenue);  // NOLINT(readability-identifier-naming

const std::type_info& FactoryImplOf(
    Venue kVenue) noexcept;  // NOLINT(readability-identifier-naming

bool IsFactoryImpl(
    const VenueFactory& factory,
    Venue kVenue) noexcept;  // NOLINT(readability-identifier-naming

const std::type_info& GatewayImplOf(
    Venue kVenue) noexcept;  // NOLINT(readability-identifier-naming

bool IsGatewayImpl(
    const OrderGateway& gateway,
    Venue kVenue) noexcept;  // NOLINT(readability-identifier-naming

#endif  // GOF23_ABSTRACT_FACTORY_H
