//
// Created by Will George on 10/8/25.
//

#include "prototype.h"

#include <iostream>

double PriceCodec::Decode(const int64_t kRaw) {
  return static_cast<double>(kRaw) / kScale;
}

int64_t PriceCodec::Encode(const double kPrice) {
  return static_cast<int64_t>(kPrice * kScale);
}

SessionCfg Hydrate(SessionCfg base, const SessionCfgOverride& override) {
  if (override.id_) base.id_ = *override.id_;
  if (override.tickers_) base.tickers_ = *override.tickers_;
  return base;
}

ConcreteParser::ConcreteParser(std::shared_ptr<ParserCfg> config)
    : cfg_(std::move(config)) {}

std::unique_ptr<IParser> ConcreteParser::Clone() const {
  const auto kCfg = std::make_shared<ParserCfg>(*cfg_);
  return std::make_unique<ConcreteParser>(kCfg);
}

void ConcreteParser::SetSession(const SessionCfg& config) { sess_ = config; }

void ConcreteParser::Parse(const std::string& raw) const {
  std::string sym = raw;
  if (!sess_.tickers_.empty()) {
    bool ok = false;
    for (const auto& t : sess_.tickers_) {
      if (sym == t) {
        ok = true;
        break;
      }
    }
    if (!ok) return;
  }
  std::cout << "[session " << sess_.id_ << "] " << sym << "\n";
}

ParserFactory::ParserFactory(std::unique_ptr<IParser> prototype,
                             SessionCfg base)
    : proto_(std::move(prototype)), base_(std::move(base)) {}

std::unique_ptr<IParser> ParserFactory::Make(
    const SessionCfgOverride& cfg) const {
  auto p = proto_->Clone();
  p->SetSession(Hydrate(base_, cfg));
  return p;
}