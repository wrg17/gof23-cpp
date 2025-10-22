//
// Created by Will George on 10/20/25.
//

#ifndef GOF23_ADAPTER_H
#define GOF23_ADAPTER_H

#include <memory>
#include <string>

enum class Side : uint8_t { kBuy, kSell };
using OrderId = std::string;

class IOrderRouter {
 public:
  virtual ~IOrderRouter() = default;
  virtual OrderId SendLimit(const std::string& sym, Side side, double price,
                            uint32_t qty) = 0;
};

namespace FixLib {
struct Session {
  static OrderId NewOrderSingle(const std::string& sym, char fixSide,
                                uint32_t qty, double price);
};
}  // namespace FixLib

namespace RestLib {
struct Client {
  static OrderId PostOrder(const std::string& sym, const std::string& side,
                           uint32_t qty, double price,
                           const std::string& timeInForce);
};
}  // namespace RestLib

class FixRouter final : public IOrderRouter {
 public:
  explicit FixRouter(std::shared_ptr<FixLib::Session> sess);

  OrderId SendLimit(const std::string& sym, Side side, double price,
                    uint32_t qty) override;

 private:
  std::shared_ptr<FixLib::Session> sess_;
};

class RestRouter final : public IOrderRouter {
 public:
  explicit RestRouter(std::shared_ptr<RestLib::Client> cli);

  OrderId SendLimit(const std::string& sym, Side side, double price,
                    uint32_t qty) override;

 private:
  std::shared_ptr<RestLib::Client> cli_;
};

class Strategy {
 public:
  explicit Strategy(std::unique_ptr<IOrderRouter> router);

  void Run() const;

 private:
  std::unique_ptr<IOrderRouter> router_;
};

#endif  // GOF23_ADAPTER_H
