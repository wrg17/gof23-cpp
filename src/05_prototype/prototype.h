//
// Created by Will George on 10/8/25.
//

#ifndef GOF23_PROTOTYPE_H
#define GOF23_PROTOTYPE_H

#include <memory>
#include <optional>
#include <string>
#include <vector>

struct PriceCodec {
  static constexpr int kScale = 10'000;
  static double Decode(int64_t kRaw);
  static int64_t Encode(double kPrice);
};

struct ParserCfg {
  PriceCodec codec_;
};

struct SessionCfg {
  std::string id_;
  std::vector<std::string> tickers_;
};

struct SessionCfgOverride {
  std::optional<std::string> id_;
  std::optional<std::vector<std::string>> tickers_;
};

SessionCfg Hydrate(SessionCfg base, const SessionCfgOverride& override);

struct IParser {
  virtual ~IParser() = default;
  [[nodiscard]] virtual std::unique_ptr<IParser> Clone() const = 0;
  virtual void SetSession(const SessionCfg& config) = 0;
  virtual void Parse(const std::string& raw) const = 0;
};

class ConcreteParser final : public IParser {
 public:
  explicit ConcreteParser(std::shared_ptr<ParserCfg> config);

  [[nodiscard]] std::unique_ptr<IParser> Clone() const override;

  void SetSession(const SessionCfg& config) override;

  void Parse(const std::string& raw) const override;

 private:
  std::shared_ptr<ParserCfg> cfg_;
  SessionCfg sess_;
};

struct IParserFactory {
  virtual ~IParserFactory() = default;
  [[nodiscard]] virtual std::unique_ptr<IParser> Make(
      const SessionCfgOverride&) const = 0;
};

class ParserFactory final : IParserFactory {
 public:
  explicit ParserFactory(std::unique_ptr<IParser> prototype, SessionCfg base);

  [[nodiscard]] std::unique_ptr<IParser> Make(
      const SessionCfgOverride& cfg) const override;

 private:
  std::unique_ptr<IParser> proto_;
  SessionCfg base_;
};

#endif  // GOF23_PROTOTYPE_H
