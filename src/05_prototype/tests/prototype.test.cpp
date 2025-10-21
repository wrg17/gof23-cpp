//
// Created by Will George on 10/20/25.
//

#include "prototype.h"

#include <gtest/gtest.h>

#include "../helpers/StdoutCaptureGuard.h"

class PrototypeSuite : public ::testing::Test {
 protected:
  const std::string kSymbol = "AAPL";
  const std::string kId = "test";
  const std::vector<std::string> kTickers{"AAPL", "MSFT"};
};

class PriceCodecSuite : public PrototypeSuite {
 protected:
  static constexpr PriceCodec kCodec{};
  static constexpr double kPrice = 1.2345;
  static constexpr int64_t kRaw = 12345;
};

TEST_F(PriceCodecSuite, shouldEncodePrice) {
  const auto kEncoded = kCodec.Encode(kPrice);
  EXPECT_EQ(kEncoded, kRaw);
}

TEST_F(PriceCodecSuite, shouldDecodePrice) {
  const auto kDecoded = kCodec.Decode(kRaw);
  EXPECT_EQ(kDecoded, kPrice);
}

class ParserCfgSuite : public PrototypeSuite {};

TEST_F(ParserCfgSuite, shouldConstruct) {
  const auto [codec_] = ParserCfg{};
  EXPECT_NE(&codec_, nullptr);
}

class SessionCfgSuite : public PrototypeSuite {};

TEST_F(SessionCfgSuite, shouldHydrateId) {
  const SessionCfg kBase{.id_ = "base", .tickers_ = {}};
  const SessionCfgOverride kOverride{.id_ = kId, .tickers_ = {}};
  const auto [id_, tickers_] = Hydrate(kBase, kOverride);
  EXPECT_EQ(id_, kId);
}

TEST_F(SessionCfgSuite, shouldHydrateTickers) {
  const SessionCfg kBase{.id_ = "base", .tickers_ = {}};
  const SessionCfgOverride kOverride{.id_ = "", .tickers_ = kTickers};
  const auto [id_, tickers_] = Hydrate(kBase, kOverride);
  EXPECT_EQ(tickers_, kTickers);
}

class ConcreteParserSuite : public PrototypeSuite {
 protected:
  std::shared_ptr<ParserCfg> cfg_ = std::make_shared<ParserCfg>();
  ConcreteParser parser_{cfg_};
};

TEST_F(ConcreteParserSuite, shouldClone) {
  const SessionCfg kSession{.id_ = kId, .tickers_ = kTickers};
  parser_.SetSession(kSession);

  StdoutCaptureGuard capture_log{};
  parser_.Parse(kSymbol);

  constexpr std::string kExpectedLogWithId = "[session test] AAPL\n";
  EXPECT_EQ(capture_log.Capture(), kExpectedLogWithId);

  const auto kClone = parser_.Clone();
  EXPECT_NE(kClone.get(), &parser_);

  capture_log.Resume();
  kClone->Parse(kSymbol);
  constexpr std::string kExpectedLogWithoutId = "[session ] AAPL\n";
  EXPECT_EQ(capture_log.Capture(), kExpectedLogWithoutId);
}

class ParserFactorySuite : public PrototypeSuite {
 protected:
  std::shared_ptr<ParserCfg> cfg_ = std::make_shared<ParserCfg>();
  SessionCfg base_{.id_ = "base", .tickers_ = {}};
  ParserFactory factory_{std::make_unique<ConcreteParser>(cfg_), base_};
};

TEST_F(ParserFactorySuite, shouldMakeWithOverride) {
  const SessionCfgOverride kOverride{.id_ = kId, .tickers_ = kTickers};
  auto parser = factory_.Make(kOverride);
  EXPECT_NE(parser, nullptr);
}
