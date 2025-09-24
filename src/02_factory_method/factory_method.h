//
// Created by Will George on 9/16/25.
//

#ifndef GOF23_FACTORY_METHOD_H
#define GOF23_FACTORY_METHOD_H

#include <concepts>
#include <format>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

template <class Derived>
concept HasStaticTypeName = requires {
  { Derived::kTypeName } -> std::convertible_to<std::string_view>;
};

class Pastry {
 public:
  virtual ~Pastry();

  [[nodiscard]] const std::string& GetFlavor() const;
  [[nodiscard]] const std::string& GetChef() const;
  [[nodiscard]] const std::string& GetBakery() const;

  [[nodiscard]] virtual std::string_view TypeName() const = 0;
  [[nodiscard]] std::string Describe() const;

 protected:
  explicit Pastry(std::string flavor, std::string chef, std::string bakery);

  std::string flavor_;
  std::string chef_;
  std::string bakery_;
};

template <class Derived>
class PastryNamed : public Pastry {
  friend Derived;

  explicit PastryNamed(std::string flavor, std::string chef, std::string bakery)
      : Pastry(std::move(flavor), std::move(chef), std::move(bakery)) {}

 public:
  [[nodiscard]] std::string_view TypeName() const final {
    static_assert(HasStaticTypeName<Derived>,
                  "Derived must declare static constexpr kTypeName convertible "
                  "to std::string_view");
    return Derived::kTypeName;
  }
};

class Croissant final : public PastryNamed<Croissant> {
 public:
  explicit Croissant(std::string flavor, std::string chef, std::string bakery);

  static constexpr std::string_view kTypeName = "croissant";
};

class Donut final : public PastryNamed<Donut> {
 public:
  explicit Donut(std::string flavor, std::string chef, std::string bakery);

  static constexpr std::string_view kTypeName = "donut";
};

class Bakery {
  enum class ServiceEmotions : std::uint8_t { kLove, kHate };
  std::string name_;
  std::string chef_;

 public:
  explicit Bakery(std::string name, std::string chef);

  virtual ~Bakery();

  [[nodiscard]] virtual std::unique_ptr<Pastry> Bake(
      const std::string& order) const = 0;

  [[nodiscard]] std::unique_ptr<Pastry> Fulfill(
      const std::string& order,
      /*NOLINT*/ bool kIsCustomerPolite) const;

  [[nodiscard]] const std::string& GetName() const;

 protected:
  static void PreheatOven();

  static void MixDough(const std::string& order);

  static void ServerWith(/*NOLINT*/ const ServiceEmotions kEmotion);

  static constexpr std::string_view kEmotionToText(
      const ServiceEmotions kEmotion) noexcept {
    switch (kEmotion) {
      case ServiceEmotions::kLove:
        return "love";
      case ServiceEmotions::kHate:
        return "hate";
      default:
        return "";
    }
  }

  [[nodiscard]] const std::string& GetChef() const;
};

class FrenchBakery final : public Bakery {
 public:
  using Bakery::Bakery;

  [[nodiscard]]
  std::unique_ptr<Pastry> Bake(const std::string& order) const override;
};

class DonutShop final : public Bakery {
 public:
  using Bakery::Bakery;

  [[nodiscard]]
  std::unique_ptr<Pastry> Bake(const std::string& order) const override;
};

#endif  // GOF23_FACTORY_METHOD_H
