//
// Created by Will George on 1/6/26.
//

#ifndef GOF23_CHAIN_OF_RESPONSIBILITY_H
#define GOF23_CHAIN_OF_RESPONSIBILITY_H

#include <memory>
#include <ostream>

#include <absl/status/status.h>

struct TradeRequest {
  std::string ticker;
  std::int64_t quantity;
  std::uint64_t price;

  friend std::ostream& operator<<(std::ostream& os,
                                  const TradeRequest& request);
};

class IHandler {
 public:
  explicit IHandler(std::shared_ptr<IHandler> next);
  virtual ~IHandler() = default;

  virtual absl::Status Operation(const TradeRequest& request) = 0;

 protected:
  absl::Status PassToNext(const TradeRequest& request) const;

  [[nodiscard]] virtual bool IsHandled(const TradeRequest& request) const = 0;

 private:
  std::shared_ptr<IHandler> next_;
};

class RiskHandler final : public IHandler {
 public:
  explicit RiskHandler(std::shared_ptr<IHandler> next);

  absl::Status Operation(const TradeRequest& request) override;

 private:
  [[nodiscard]] bool IsHandled(const TradeRequest& request) const override;
};

class ExecutionHandler final : public IHandler {
 public:
  explicit ExecutionHandler(std::shared_ptr<IHandler> next);

  absl::Status Operation(const TradeRequest& request) override;

 private:
  [[nodiscard]] bool IsHandled(const TradeRequest& request) const override;
};

#endif  // GOF23_CHAIN_OF_RESPONSIBILITY_H
