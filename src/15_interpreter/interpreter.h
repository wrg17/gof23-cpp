//
// Created by Will George on 2/15/26.
//

#ifndef GOF23_INTERPRETER_H
#define GOF23_INTERPRETER_H

#include <cstdint>
#include <memory>
#include <string>

#include <absl/container/flat_hash_map.h>
#include <absl/status/statusor.h>

// Context
class Context {
 public:
  Context();
  void SetVariable(const std::string& name, std::int64_t value);
  [[nodiscard]] absl::StatusOr<std::int64_t> GetVariable(
      const std::string& name) const;

 private:
  absl::flat_hash_map<std::string, std::int64_t> variables_;
};

// Abstract Expression
class IExpression {
 public:
  virtual ~IExpression() = default;

  [[nodiscard]] virtual absl::StatusOr<std::int64_t> Interpret(
      Context& context) const = 0;
};

// Terminal Expressions
class NumberExpression final : public IExpression {
 public:
  explicit NumberExpression(std::int64_t value);
  [[nodiscard]] absl::StatusOr<std::int64_t> Interpret(
      Context& context) const override;

 private:
  std::int64_t value_;
};

class VariableExpression final : public IExpression {
 public:
  explicit VariableExpression(std::string name);

  [[nodiscard]] absl::StatusOr<std::int64_t> Interpret(
      Context& context) const override;

 private:
  std::string name_;
};

// Non-terminal Expression
class AddExpression final : public IExpression {
 public:
  AddExpression(std::unique_ptr<IExpression> left,
                std::unique_ptr<IExpression> right);

  [[nodiscard]] absl::StatusOr<std::int64_t> Interpret(
      Context& context) const override;

 private:
  std::unique_ptr<IExpression> left_;
  std::unique_ptr<IExpression> right_;
};

class SubtractExpression final : public IExpression {
 public:
  SubtractExpression(std::unique_ptr<IExpression> left,
                     std::unique_ptr<IExpression> right);

  [[nodiscard]] absl::StatusOr<std::int64_t> Interpret(
      Context& context) const override;

 private:
  std::unique_ptr<IExpression> left_;
  std::unique_ptr<IExpression> right_;
};

class MultiplyExpression final : public IExpression {
 public:
  MultiplyExpression(std::unique_ptr<IExpression> left,
                     std::unique_ptr<IExpression> right);

  [[nodiscard]] absl::StatusOr<std::int64_t> Interpret(
      Context& context) const override;

 private:
  std::unique_ptr<IExpression> left_;
  std::unique_ptr<IExpression> right_;
};

class DivideExpression final : public IExpression {
 public:
  DivideExpression(std::unique_ptr<IExpression> left,
                   std::unique_ptr<IExpression> right);

  [[nodiscard]] absl::StatusOr<std::int64_t> Interpret(
      Context& context) const override;

 private:
  std::unique_ptr<IExpression> left_;
  std::unique_ptr<IExpression> right_;
};

// Evaluates a string expression and prints the result
void EvaluateExpression(const std::string& expr_str, Context& context);

// Parser - converts string expressions to expression trees
class ExpressionParser {
 public:
  [[nodiscard]] static absl::StatusOr<std::unique_ptr<IExpression>> Parse(
      const std::string& expression);

 private:
  explicit ExpressionParser(std::string expression);

  [[nodiscard]] absl::StatusOr<std::unique_ptr<IExpression>> ParseExpr(
      int min_precedence);
  [[nodiscard]] absl::StatusOr<std::unique_ptr<IExpression>> ParsePrimary();

  [[nodiscard]] static int GetPrecedence(char op);
  [[nodiscard]] static std::unique_ptr<IExpression> MakeBinaryExpr(
      char op, std::unique_ptr<IExpression> left,
      std::unique_ptr<IExpression> right);

  void SkipWhitespace();
  [[nodiscard]] char Peek() const;
  char Consume();
  [[nodiscard]] bool IsAtEnd() const;
  [[nodiscard]] static bool IsOperator(char ch);

  std::string expression_;
  std::size_t pos_;
};

#endif  // GOF23_INTERPRETER_H
