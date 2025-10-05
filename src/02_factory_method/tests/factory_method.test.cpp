//
// Created by Will George on 9/16/25.
//

#include "factory_method.h"

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

#include <gtest/gtest.h>

class TestPastry final : public PastryNamed<TestPastry> {
 public:
  explicit TestPastry(std::string flavor, std::string chef, std::string bakery)
      : PastryNamed(std::move(flavor), std::move(chef), std::move(bakery)) {}

  static constexpr std::string_view kTypeName = "test pastry";
};

class TestBakery final : public Bakery {
 public:
  TestBakery(std::string name, std::string chef)
      : Bakery(std::move(name), std::move(chef)) {}

  [[nodiscard]] std::unique_ptr<Pastry> Bake(
      const std::string& order) const override {
    return std::make_unique<TestPastry>(order, GetChef(), GetName());
  }
};

TEST(PastryDescribe, CroissantMentionsChef) {
  const Croissant kAlmondCroissant("Almond", "Claire", "Mom's Bakery");
  EXPECT_EQ(kAlmondCroissant.Describe(),
            "A almond croissant made by chef Claire at Mom's Bakery");
}

TEST(PastryDescribe, DonutMentionsChef) {
  const Donut kChocolateDonut("Chocolate", "Bob", "Bob's Donut Shop");
  EXPECT_EQ(kChocolateDonut.Describe(),
            "A chocolate donut made by chef Bob at Bob's Donut Shop");
}

TEST(BakeryBasics, GettersReturnCtorValues) {
  const TestBakery kCornerBakery("Corner Bakery", "Alice");
  EXPECT_EQ(kCornerBakery.GetName(), "Corner Bakery");
}

TEST(FulfillFlow, ProducesPastryAndLogsPolite) {
  const TestBakery kCornerBakery("Corner Bakery", "Alice");
  constexpr bool kIsCustomerPolite = true;

  testing::internal::CaptureStdout();

  const auto kPastry = kCornerBakery.Fulfill("Bagel", kIsCustomerPolite);

  const std::string kOut = testing::internal::GetCapturedStdout();

  ASSERT_NE(kPastry, nullptr);
  EXPECT_EQ(kPastry->Describe(),
            "A bagel test pastry made by chef Alice at Corner Bakery");

  EXPECT_NE(kOut.find("Firing up the oven!"), std::string::npos);
  EXPECT_NE(kOut.find("Mixing Bagel dough"), std::string::npos);
  EXPECT_NE(kOut.find("Serving with love!"), std::string::npos);
}

TEST(FulfillFlow, ProducesPastryAndLogsRude) {
  const TestBakery kCornerBakery("Corner Bakery", "Alice");
  constexpr bool kIsCustomerPolite = false;

  testing::internal::CaptureStdout();

  const auto kPastry = kCornerBakery.Fulfill("Muffin", kIsCustomerPolite);

  const std::string kOut = testing::internal::GetCapturedStdout();

  ASSERT_NE(kPastry, nullptr);
  EXPECT_EQ(kPastry->Describe(),
            "A muffin test pastry made by chef Alice at Corner Bakery");

  EXPECT_NE(kOut.find("Firing up the oven!"), std::string::npos);
  EXPECT_NE(kOut.find("Mixing Muffin dough"), std::string::npos);
  EXPECT_NE(kOut.find("Serving with hate!"), std::string::npos);
}

TEST(ConcreteBakeries, FrenchBakeryBakesCroissant) {
  const FrenchBakery kJuliaBakery("Le Pain", "Julia");

  const auto kPastry = kJuliaBakery.Bake("Butter");
  ASSERT_NE(kPastry, nullptr);

  EXPECT_NE(dynamic_cast<Croissant*>(kPastry.get()), nullptr);
  EXPECT_EQ(kPastry->Describe(),
            "A butter croissant made by chef Julia at Le Pain");
}

TEST(ConcreteBakeries, DonutShopBakesDonut) {
  const DonutShop kDonutShop("Gas Station", "John");

  const auto kPastry = kDonutShop.Bake("Glazed");
  ASSERT_NE(kPastry, nullptr);

  EXPECT_NE(dynamic_cast<Donut*>(kPastry.get()), nullptr);
  EXPECT_EQ(kPastry->Describe(),
            "A glazed donut made by chef John at Gas Station");
}