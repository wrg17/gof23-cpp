//
// Created by Will George on 2/15/26.
//

#include "interpreter.h"

#include <cctype>
#include <iostream>
#include <limits>
#include <utility>

namespace {
constexpr std::int64_t kDecimalBase = 10;
constexpr int kAddSubPrecedence = 1;
constexpr int kMulDivPrecedence = 2;
}  // namespace

// Context

Context::Context() = default;

void Context::SetVariable(const std::string& name, const std::int64_t value) {
  variables_[name] = value;
}

absl::StatusOr<std::int64_t> Context::GetVariable(
    const std::string& name) const {
  const auto value = variables_.find(name);
  if (value == variables_.end()) {
    return absl::NotFoundError("variable not found");
  }
  return value->second;
}

// Terminal Expressions

NumberExpression::NumberExpression(const std::int64_t value) : value_(value) {}

absl::StatusOr<std::int64_t> NumberExpression::Interpret(
    Context& /*context*/) const {
  return value_;
}

VariableExpression::VariableExpression(std::string name)
    : name_(std::move(name)) {}

absl::StatusOr<std::int64_t> VariableExpression::Interpret(
    Context& context) const {
  return context.GetVariable(name_);
}

// Non-terminal Expressions

AddExpression::AddExpression(std::unique_ptr<IExpression> left,
                             std::unique_ptr<IExpression> right)
    : left_(std::move(left)), right_(std::move(right)) {}

absl::StatusOr<std::int64_t> AddExpression::Interpret(Context& context) const {
  auto left_value = left_->Interpret(context);
  if (!left_value.ok()) {
    return left_value.status();
  }

  auto right_value = right_->Interpret(context);
  if (!right_value.ok()) {
    return right_value.status();
  }

  if (*right_value > 0 &&
      *left_value > std::numeric_limits<std::int64_t>::max() - *right_value) {
    return absl::OutOfRangeError("addition overflow");
  }
  if (*right_value < 0 &&
      *left_value < std::numeric_limits<std::int64_t>::min() - *right_value) {
    return absl::OutOfRangeError("addition underflow");
  }

  return *left_value + *right_value;
}

SubtractExpression::SubtractExpression(std::unique_ptr<IExpression> left,
                                       std::unique_ptr<IExpression> right)
    : left_(std::move(left)), right_(std::move(right)) {}

absl::StatusOr<std::int64_t> SubtractExpression::Interpret(
    Context& context) const {
  auto left_value = left_->Interpret(context);
  if (!left_value.ok()) {
    return left_value.status();
  }

  auto right_value = right_->Interpret(context);
  if (!right_value.ok()) {
    return right_value.status();
  }

  if (*right_value < 0 &&
      *left_value > std::numeric_limits<std::int64_t>::max() + *right_value) {
    return absl::OutOfRangeError("subtraction overflow");
  }
  if (*right_value > 0 &&
      *left_value < std::numeric_limits<std::int64_t>::min() + *right_value) {
    return absl::OutOfRangeError("subtraction underflow");
  }

  return *left_value - *right_value;
}

MultiplyExpression::MultiplyExpression(std::unique_ptr<IExpression> left,
                                       std::unique_ptr<IExpression> right)
    : left_(std::move(left)), right_(std::move(right)) {}

absl::StatusOr<std::int64_t> MultiplyExpression::Interpret(
    Context& context) const {
  auto left_value = left_->Interpret(context);
  if (!left_value.ok()) {
    return left_value.status();
  }

  auto right_value = right_->Interpret(context);
  if (!right_value.ok()) {
    return right_value.status();
  }

  if (*left_value != 0 && *right_value != 0) {
    if (*left_value > 0 && *right_value > 0 &&
        *left_value > std::numeric_limits<std::int64_t>::max() / *right_value) {
      return absl::OutOfRangeError("multiplication overflow");
    }
    if (*left_value < 0 && *right_value < 0 &&
        *left_value < std::numeric_limits<std::int64_t>::max() / *right_value) {
      return absl::OutOfRangeError("multiplication overflow");
    }
    if (*left_value > 0 && *right_value < 0 &&
        *right_value < std::numeric_limits<std::int64_t>::min() / *left_value) {
      return absl::OutOfRangeError("multiplication underflow");
    }
    if (*left_value < 0 && *right_value > 0 &&
        *left_value < std::numeric_limits<std::int64_t>::min() / *right_value) {
      return absl::OutOfRangeError("multiplication underflow");
    }
  }

  return *left_value * *right_value;
}

DivideExpression::DivideExpression(std::unique_ptr<IExpression> left,
                                   std::unique_ptr<IExpression> right)
    : left_(std::move(left)), right_(std::move(right)) {}

absl::StatusOr<std::int64_t> DivideExpression::Interpret(
    Context& context) const {
  auto left_value = left_->Interpret(context);
  if (!left_value.ok()) {
    return left_value.status();
  }

  auto right_value = right_->Interpret(context);
  if (!right_value.ok()) {
    return right_value.status();
  }

  if (*right_value == 0) {
    return absl::InvalidArgumentError("divide by zero");
  }

  if (*left_value == std::numeric_limits<std::int64_t>::min() &&
      *right_value == -1) {
    return absl::OutOfRangeError("division overflow");
  }

  if (std::abs(*left_value) % std::abs(*right_value) != 0) {
    return absl::InvalidArgumentError(
        "The expression is closed on the integers");
  }

  return *left_value / *right_value;
}

// EvaluateExpression

void EvaluateExpression(const std::string& expr_str, Context& context) {
  std::cout << "Expression: " << expr_str << "\n";
  const auto kParsedExpr = ExpressionParser::Parse(expr_str);
  if (!kParsedExpr.ok()) {
    std::cerr << "Parse error: " << kParsedExpr.status().message() << "\n\n";
    return;
  }

  if (const auto kResult = (*kParsedExpr)->Interpret(context); kResult.ok()) {
    std::cout << "Result: " << *kResult << "\n\n";
  } else {
    std::cerr << "Error: " << kResult.status().message() << "\n\n";
  }
}

// Parser

ExpressionParser::ExpressionParser(std::string expression)
    : expression_(std::move(expression)), pos_(0) {}

absl::StatusOr<std::unique_ptr<IExpression>> ExpressionParser::Parse(
    const std::string& expression) {
  ExpressionParser parser(expression);
  auto result = parser.ParseExpr(0);
  if (!result.ok()) {
    return result.status();
  }
  parser.SkipWhitespace();
  if (!parser.IsAtEnd()) {
    return absl::InvalidArgumentError("unexpected characters after expression");
  }
  return result;
}

int ExpressionParser::GetPrecedence(const char op) {
  switch (op) {
    case '+':
    case '-':
      return kAddSubPrecedence;
    case '*':
    case '/':
      return kMulDivPrecedence;
    default:
      return 0;
  }
}

std::unique_ptr<IExpression> ExpressionParser::MakeBinaryExpr(
    const char op, std::unique_ptr<IExpression> left,
    std::unique_ptr<IExpression> right) {
  switch (op) {
    case '+':
      return std::make_unique<AddExpression>(std::move(left), std::move(right));
    case '-':
      return std::make_unique<SubtractExpression>(std::move(left),
                                                  std::move(right));
    case '*':
      return std::make_unique<MultiplyExpression>(std::move(left),
                                                  std::move(right));
    case '/':
      return std::make_unique<DivideExpression>(std::move(left),
                                                std::move(right));
    default:
      return nullptr;
  }
}

bool ExpressionParser::IsOperator(const char ch) {
  return ch == '+' || ch == '-' || ch == '*' || ch == '/';
}

absl::StatusOr<std::unique_ptr<IExpression>> ExpressionParser::ParseExpr(
    const int min_precedence) {
  auto left = ParsePrimary();
  if (!left.ok()) {
    return left.status();
  }

  SkipWhitespace();
  while (!IsAtEnd() && IsOperator(Peek()) &&
         GetPrecedence(Peek()) >= min_precedence) {
    const char kOperator = Consume();
    const int kNextMinPrecedence = GetPrecedence(kOperator) + 1;
    auto right = ParseExpr(kNextMinPrecedence);
    if (!right.ok()) {
      return right.status();
    }
    left = MakeBinaryExpr(kOperator, std::move(*left), std::move(*right));
  }

  return left;
}

absl::StatusOr<std::unique_ptr<IExpression>> ExpressionParser::ParsePrimary() {
  SkipWhitespace();

  if (IsAtEnd()) {
    return absl::InvalidArgumentError("unexpected end of expression");
  }

  if (Peek() == '(') {
    Consume();
    auto inner_expr = ParseExpr(0);
    if (!inner_expr.ok()) {
      return inner_expr.status();
    }
    SkipWhitespace();
    if (IsAtEnd() || Peek() != ')') {
      return absl::InvalidArgumentError("missing closing parenthesis");
    }
    Consume();
    return inner_expr;
  }

  if (std::isdigit(static_cast<unsigned char>(Peek())) != 0 ||
      (Peek() == '-' && pos_ + 1 < expression_.size() &&
       std::isdigit(static_cast<unsigned char>(expression_[pos_ + 1])) != 0)) {
    bool negative = false;
    if (Peek() == '-') {
      negative = true;
      Consume();
    }

    std::int64_t value = 0;
    while (!IsAtEnd() &&
           std::isdigit(static_cast<unsigned char>(Peek())) != 0) {
      value = value * kDecimalBase + (Consume() - '0');
    }

    if (negative) {
      value = -value;
    }

    return std::make_unique<NumberExpression>(value);
  }

  if (std::isalpha(static_cast<unsigned char>(Peek())) != 0 || Peek() == '_') {
    std::string name;
    while (!IsAtEnd() &&
           (std::isalnum(static_cast<unsigned char>(Peek())) != 0 ||
            Peek() == '_')) {
      name += Consume();
    }
    return std::make_unique<VariableExpression>(name);
  }

  return absl::InvalidArgumentError("unexpected character in expression");
}

void ExpressionParser::SkipWhitespace() {
  while (!IsAtEnd() && std::isspace(static_cast<unsigned char>(Peek())) != 0) {
    ++pos_;
  }
}

char ExpressionParser::Peek() const { return expression_[pos_]; }

char ExpressionParser::Consume() { return expression_[pos_++]; }

bool ExpressionParser::IsAtEnd() const { return pos_ >= expression_.size(); }
