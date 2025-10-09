//
// Created by Will George on 10/4/25.
//

#ifndef GOF23_BUILDER_H
#define GOF23_BUILDER_H

#include <chrono>
#include <string>
#include <string_view>
#include <vector>

// NOLINTBEGIN(readability-identifier-naming)
enum class Right : std::uint8_t { kCall, kPut };
enum class Side : std::uint8_t { kLong, kShort };

struct Option {
  explicit Option(std::string ticker, std::uint64_t kExpDate, Right kRight,
                  double kStrike, Side kSide, double kPremium);

  bool operator==(const Option&) const;

  std::string ticker_;
  std::uint64_t exp_date_{};
  Right right_;
  double strike_{};
  Side side_;
  double premium_{};
};

struct Strategy {
  explicit Strategy();
  explicit Strategy(std::string name);

  std::string name_;
  std::vector<Option> legs_;
};

struct IStrategyBuilder {
  virtual ~IStrategyBuilder() = default;
  virtual IStrategyBuilder& Reset(const std::string& name) = 0;
  virtual IStrategyBuilder& Header(const std::string& ticker,
                                   std::uint64_t kExpDate) = 0;
  virtual IStrategyBuilder& LongCall(double kStrike, double kPremium) = 0;
  virtual IStrategyBuilder& ShortCall(double kStrike, double kPremium) = 0;
  virtual IStrategyBuilder& LongPut(double kStrike, double kPremium) = 0;
  virtual IStrategyBuilder& ShortPut(double kStrike, double kPremium) = 0;
  virtual Strategy Build() = 0;
};

class SimpleStrategyBuilder final : public IStrategyBuilder {
  Strategy strategy_;
  std::string ticker_;
  std::uint64_t exp_date_{};

  [[nodiscard]] IStrategyBuilder& Add(Right kRight, Side kSide, double kStrike,
                                      double kPremium);

 public:
  [[nodiscard]] IStrategyBuilder& Reset(const std::string& name) override;
  [[nodiscard]] IStrategyBuilder& Header(const std::string& ticker,
                                         std::uint64_t kExpDate) override;
  [[nodiscard]] IStrategyBuilder& LongCall(double kStrike,
                                           double kPremium) override;
  [[nodiscard]] IStrategyBuilder& ShortCall(double kStrike,
                                            double kPremium) override;
  [[nodiscard]] IStrategyBuilder& LongPut(double kStrike,
                                          double kPremium) override;
  [[nodiscard]] IStrategyBuilder& ShortPut(double kStrike,
                                           double kPremium) override;
  [[nodiscard]] Strategy Build() override;
};

class StrategyDirector final {
  IStrategyBuilder& builder_;

 public:
  explicit StrategyDirector(IStrategyBuilder& builder);

  [[nodiscard]] Strategy IronCondor(const std::string& ticker,
                                    const std::string& exp_date,
                                    double kStrikePBuy, double kPremiumPBuy,
                                    double kStrikePSell, double kPremiumPSell,
                                    double kStrikeCSell, double kPremiumCSell,
                                    double kStrikeCBuy,
                                    double kPremiumCBuy) const;
};

inline uint64_t DateStringToUnixTimestamp(const std::string_view in_date) {
  auto to_u = [](const std::string_view kInDate) -> unsigned {
    unsigned result = 0;
    for (const char kDigit : kInDate) {
      constexpr auto kStrBase = 10;
      result = result * kStrBase + (kDigit - '0');
    }
    return result;
  };

  const int kYear = static_cast<int>(to_u(in_date.substr(0, 4)));
  const unsigned kMonth = to_u(in_date.substr(5, 2));
  const unsigned kDay = to_u(in_date.substr(8, 2));

  using std::chrono::day;
  using std::chrono::duration_cast;
  using std::chrono::month;
  using std::chrono::seconds;
  using std::chrono::sys_days;
  using std::chrono::year;
  using std::chrono::year_month_day;

  const year_month_day kYmd{year{kYear} / month{kMonth} / day{kDay}};
  const sys_days kYmdSys{kYmd};

  const uint64_t kEpoch =
      duration_cast<seconds>(kYmdSys.time_since_epoch()).count();

  return kEpoch;
}
// NOLINTEND(readability-identifier-naming)

#endif  // GOF23_BUILDER_H
