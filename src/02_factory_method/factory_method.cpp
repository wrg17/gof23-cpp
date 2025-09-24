//
// Created by Will George on 9/16/25.
//

#include "factory_method.h"

#include <format>
#include <iostream>
#include <memory>
#include <utility>

#include <absl/strings/ascii.h>
#include <absl/strings/str_join.h>
#include <absl/strings/str_split.h>

std::string FormatProperNoun(const std::string& noun) {
  if (noun.empty()) return std::string{};

  constexpr absl::string_view kAsciiWhiteSpace = " \t\n\r\f\v";
  const auto kWhiteSpace = absl::ByAnyChar(kAsciiWhiteSpace);

  std::vector<std::string> parts =
      absl::StrSplit(noun, kWhiteSpace, absl::SkipEmpty());

  for (auto& part : parts) {
    part = absl::AsciiStrToLower(part);
    part[0] = absl::ascii_toupper(static_cast<unsigned char>(part[0]));
  }

  return absl::StrJoin(parts, " ");
}

Pastry::Pastry(std::string flavor, std::string chef, std::string bakery)
    : flavor_(std::move(flavor)),
      chef_(std::move(chef)),
      bakery_(std::move(bakery)) {
  absl::AsciiStrToLower(&flavor_);
}

Pastry::~Pastry() = default;

const std::string& Pastry::GetFlavor() const { return flavor_; }

const std::string& Pastry::GetChef() const { return chef_; }

const std::string& Pastry::GetBakery() const { return bakery_; }

std::string Pastry::Describe() const {
  return std::format("A {} {} made by chef {} at {}", GetFlavor(), TypeName(),
                     GetChef(), GetBakery());
}

Croissant::Croissant(std::string flavor, std::string chef, std::string bakery)
    : PastryNamed(std::move(flavor), std::move(chef), std::move(bakery)) {}

Donut::Donut(std::string flavor, std::string chef, std::string bakery)
    : PastryNamed(std::move(flavor), std::move(chef), std::move(bakery)) {}

Bakery::Bakery(std::string name, std::string chef)
    : name_(std::move(name)), chef_(std::move(chef)) {
  name_ = FormatProperNoun(name_);
  chef_ = FormatProperNoun(chef_);
}

Bakery::~Bakery() = default;

std::unique_ptr<Pastry> Bakery::Fulfill(const std::string& order,
                                        const bool kIsCustomerPolite) const {
  PreheatOven();
  MixDough(order);

  auto pastry = Bake(absl::AsciiStrToLower(order));

  ServerWith(kIsCustomerPolite ? ServiceEmotions::kLove
                               : ServiceEmotions::kHate);

  return pastry;
}

const std::string& Bakery::GetName() const { return name_; }

const std::string& Bakery::GetChef() const { return chef_; }

void Bakery::PreheatOven() { std::cout << "Firing up the oven!\n"; }

void Bakery::MixDough(const std::string& order) {
  std::cout << std::format("Mixing {} dough\n", order);
}

void Bakery::ServerWith(const ServiceEmotions kEmotion) {
  std::cout << std::format("Serving with {}!\n", kEmotionToText(kEmotion));
}

std::unique_ptr<Pastry> FrenchBakery::Bake(const std::string& order) const {
  return std::make_unique<Croissant>(order, GetChef(), GetName());
}

std::unique_ptr<Pastry> DonutShop::Bake(const std::string& order) const {
  return std::make_unique<Donut>(order, GetChef(), GetName());
}