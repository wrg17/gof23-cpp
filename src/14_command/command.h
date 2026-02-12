//
// Created by Will George on 1/8/26.
//

#ifndef GOF23_COMMAND_H
#define GOF23_COMMAND_H

// NOLINTBEGIN(readability-identifier-naming)

#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include <absl/container/flat_hash_map.h>
#include <absl/status/status.h>
#include <absl/status/statusor.h>

inline constexpr std::uint64_t kPriceMultiplier = 10000;

std::string FormatPrice(std::uint64_t kValue);

enum class Side : std::uint8_t { kBuy, kSell };

class Trade {
 public:
  Trade(std::string ticker, std::uint64_t quantity, Side side);

  [[nodiscard]] Trade GetInverse() const;

  [[nodiscard]] const std::string& GetTicker() const;

  [[nodiscard]] std::uint64_t GetQuantity() const;

  [[nodiscard]] Side GetSide() const;

  [[nodiscard]] std::string GetSideString() const;

  [[nodiscard]] std::string ToString() const;

  friend std::ostream& operator<<(std::ostream& out, const Trade& request);

 private:
  std::string ticker_;
  std::uint64_t quantity_;
  Side side_;
};

struct Holding {
  std::uint64_t quantity;
  std::uint64_t avg_cost;
};

class Portfolio final {
 public:
  explicit Portfolio(std::uint64_t kId);

  [[nodiscard]] absl::Status UpdateHolding(const Trade& trade,
                                           std::uint64_t kPrice);

  std::uint64_t Deposit(std::uint64_t amount);

  [[nodiscard]] std::uint64_t GetBalance() const;

  [[nodiscard]] std::uint64_t GetId() const;

 private:
  [[nodiscard]] absl::Status BuyHolding(const Trade& trade,
                                        std::uint64_t kPrice);

  [[nodiscard]] absl::Status SellHolding(const Trade& trade,
                                         std::uint64_t kPrice);

  std::uint64_t id_;
  std::uint64_t balance_{0};
  absl::flat_hash_map<std::string, Holding> holdings_;
};

// Receiver
class OrderBook final {
 public:
  explicit OrderBook(std::uint64_t kId);

  [[nodiscard]] absl::Status ExecuteTrade(
      const std::shared_ptr<Portfolio>& portfolio, const Trade& trade) const;

  void SetPrice(const std::string& ticker, std::uint64_t kPrice);

  [[nodiscard]] absl::StatusOr<std::uint64_t> GetPrice(
      const std::string& ticker) const;

 private:
  std::uint64_t id_;
  absl::flat_hash_map<std::string, std::uint64_t> prices_;
};

// Command
class ICommand {
 public:
  virtual ~ICommand() = default;

  [[nodiscard]] absl::Status Execute();

  [[nodiscard]] absl::Status Undo();

  [[nodiscard]] virtual std::string Description() const = 0;

 protected:
  [[nodiscard]] virtual absl::Status DoExecute() = 0;

  [[nodiscard]] virtual absl::Status DoUndo() = 0;

 private:
  bool executed_{false};
};

// Concrete Command
class StockTradeCommand final : public ICommand {
 public:
  StockTradeCommand(std::shared_ptr<OrderBook> order_book,
                    std::shared_ptr<Portfolio> portfolio, Trade trade);

  [[nodiscard]] std::string Description() const override;

 protected:
  [[nodiscard]] absl::Status DoExecute() override;

  [[nodiscard]] absl::Status DoUndo() override;

 private:
  std::shared_ptr<OrderBook> order_book_;
  std::shared_ptr<Portfolio> portfolio_;
  Trade trade_;
};

// Invoker
class TradingEngine final {
 public:
  explicit TradingEngine(std::uint64_t kId);

  [[nodiscard]] absl::Status SubmitOrder(std::unique_ptr<ICommand> command);

  [[nodiscard]] absl::Status UndoLast();

  [[nodiscard]] const std::vector<std::string>& GetAuditLog() const;

 private:
  std::uint64_t id_;

  std::vector<std::string> audit_log_;

  std::vector<std::unique_ptr<ICommand>> history_;
};

// NOLINTEND(readability-identifier-naming)

#endif  // GOF23_COMMAND_H
