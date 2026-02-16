//
// Created by Will George on 2/15/26.
//

#include "interpreter.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <limits>
#include <memory>
#include <string>

class ContextSuite : public ::testing::Test {
 protected:
  static constexpr std::int64_t kValue = 42;
  static constexpr std::int64_t kOtherValue = 100;
};

TEST_F(ContextSuite, ShouldSetAndGetVariable) {
  Context context;

  context.SetVariable("x", kValue);
  const auto kResult = context.GetVariable("x");

  ASSERT_TRUE(kResult.ok());
  EXPECT_EQ(*kResult, kValue);
}

TEST_F(ContextSuite, ShouldReturnErrorForUndefinedVariable) {
  Context context;

  const auto kResult = context.GetVariable("undefined");

  EXPECT_FALSE(kResult.ok());
  EXPECT_EQ(kResult.status().code(), absl::StatusCode::kNotFound);
}

TEST_F(ContextSuite, ShouldOverwriteVariable) {
  Context context;
  context.SetVariable("x", kValue);

  context.SetVariable("x", kOtherValue);
  const auto kResult = context.GetVariable("x");

  ASSERT_TRUE(kResult.ok());
  EXPECT_EQ(*kResult, kOtherValue);
}

TEST_F(ContextSuite, ShouldStoreMultipleVariables) {
  Context context;

  context.SetVariable("x", kValue);
  context.SetVariable("y", kOtherValue);

  const auto kResultX = context.GetVariable("x");
  const auto kResultY = context.GetVariable("y");

  ASSERT_TRUE(kResultX.ok());
  ASSERT_TRUE(kResultY.ok());
  EXPECT_EQ(*kResultX, kValue);
  EXPECT_EQ(*kResultY, kOtherValue);
}

class NumberExpressionSuite : public ::testing::Test {
 protected:
  static constexpr std::int64_t kPositive = 42;
  static constexpr std::int64_t kNegative = -17;
  static constexpr std::int64_t kZero = 0;
};

TEST_F(NumberExpressionSuite, ShouldInterpretPositiveNumber) {
  Context context;
  const NumberExpression kExpr{kPositive};

  const auto kResult = kExpr.Interpret(context);

  ASSERT_TRUE(kResult.ok());
  EXPECT_EQ(*kResult, kPositive);
}

TEST_F(NumberExpressionSuite, ShouldInterpretNegativeNumber) {
  Context context;
  const NumberExpression kExpr{kNegative};

  const auto kResult = kExpr.Interpret(context);

  ASSERT_TRUE(kResult.ok());
  EXPECT_EQ(*kResult, kNegative);
}

TEST_F(NumberExpressionSuite, ShouldInterpretZero) {
  Context context;
  const NumberExpression kExpr{kZero};

  const auto kResult = kExpr.Interpret(context);

  ASSERT_TRUE(kResult.ok());
  EXPECT_EQ(*kResult, kZero);
}

class VariableExpressionSuite : public ::testing::Test {
 protected:
  static constexpr std::int64_t kValue = 99;
};

TEST_F(VariableExpressionSuite, ShouldInterpretDefinedVariable) {
  Context context;
  context.SetVariable("x", kValue);
  const VariableExpression kExpr{"x"};

  const auto kResult = kExpr.Interpret(context);

  ASSERT_TRUE(kResult.ok());
  EXPECT_EQ(*kResult, kValue);
}

TEST_F(VariableExpressionSuite, ShouldReturnErrorForUndefinedVariable) {
  Context context;
  const VariableExpression kExpr{"undefined"};

  const auto kResult = kExpr.Interpret(context);

  EXPECT_FALSE(kResult.ok());
  EXPECT_EQ(kResult.status().code(), absl::StatusCode::kNotFound);
}

class AddExpressionSuite : public ::testing::Test {
 protected:
  static constexpr std::int64_t kLeft = 10;
  static constexpr std::int64_t kRight = 5;
};

TEST_F(AddExpressionSuite, ShouldAddTwoNumbers) {
  Context context;
  const auto kAddExpr = std::make_unique<AddExpression>(
      std::make_unique<NumberExpression>(kLeft),
      std::make_unique<NumberExpression>(kRight));

  const auto kResult = kAddExpr->Interpret(context);

  ASSERT_TRUE(kResult.ok());
  EXPECT_EQ(*kResult, kLeft + kRight);
}

TEST_F(AddExpressionSuite, ShouldAddVariables) {
  Context context;
  context.SetVariable("x", kLeft);
  context.SetVariable("y", kRight);
  const auto kAddExpr = std::make_unique<AddExpression>(
      std::make_unique<VariableExpression>("x"),
      std::make_unique<VariableExpression>("y"));

  const auto kResult = kAddExpr->Interpret(context);

  ASSERT_TRUE(kResult.ok());
  EXPECT_EQ(*kResult, kLeft + kRight);
}

TEST_F(AddExpressionSuite, ShouldPropagateLeftError) {
  Context context;
  context.SetVariable("y", kRight);
  const auto kAddExpr = std::make_unique<AddExpression>(
      std::make_unique<VariableExpression>("undefined"),
      std::make_unique<VariableExpression>("y"));

  const auto kResult = kAddExpr->Interpret(context);

  EXPECT_FALSE(kResult.ok());
}

class SubtractExpressionSuite : public ::testing::Test {
 protected:
  static constexpr std::int64_t kLeft = 10;
  static constexpr std::int64_t kRight = 5;
};

TEST_F(SubtractExpressionSuite, ShouldSubtractTwoNumbers) {
  Context context;
  const auto kSubExpr = std::make_unique<SubtractExpression>(
      std::make_unique<NumberExpression>(kLeft),
      std::make_unique<NumberExpression>(kRight));

  const auto kResult = kSubExpr->Interpret(context);

  ASSERT_TRUE(kResult.ok());
  EXPECT_EQ(*kResult, kLeft - kRight);
}

TEST_F(SubtractExpressionSuite, ShouldProduceNegativeResult) {
  Context context;
  const auto kSubExpr = std::make_unique<SubtractExpression>(
      std::make_unique<NumberExpression>(kRight),
      std::make_unique<NumberExpression>(kLeft));

  const auto kResult = kSubExpr->Interpret(context);

  ASSERT_TRUE(kResult.ok());
  EXPECT_EQ(*kResult, kRight - kLeft);
}

class MultiplyExpressionSuite : public ::testing::Test {
 protected:
  static constexpr std::int64_t kLeft = 10;
  static constexpr std::int64_t kRight = 5;
};

TEST_F(MultiplyExpressionSuite, ShouldMultiplyTwoNumbers) {
  Context context;
  const auto kMulExpr = std::make_unique<MultiplyExpression>(
      std::make_unique<NumberExpression>(kLeft),
      std::make_unique<NumberExpression>(kRight));

  const auto kResult = kMulExpr->Interpret(context);

  ASSERT_TRUE(kResult.ok());
  EXPECT_EQ(*kResult, kLeft * kRight);
}

TEST_F(MultiplyExpressionSuite, ShouldMultiplyByZero) {
  Context context;
  const auto kMulExpr = std::make_unique<MultiplyExpression>(
      std::make_unique<NumberExpression>(kLeft),
      std::make_unique<NumberExpression>(0));

  const auto kResult = kMulExpr->Interpret(context);

  ASSERT_TRUE(kResult.ok());
  EXPECT_EQ(*kResult, 0);
}

class DivideExpressionSuite : public ::testing::Test {
 protected:
  static constexpr std::int64_t kDividend = 10;
  static constexpr std::int64_t kDivisor = 2;
  static constexpr std::int64_t kNonDivisor = 3;
};

TEST_F(DivideExpressionSuite, ShouldDivideTwoNumbers) {
  Context context;
  const auto kDivExpr = std::make_unique<DivideExpression>(
      std::make_unique<NumberExpression>(kDividend),
      std::make_unique<NumberExpression>(kDivisor));

  const auto kResult = kDivExpr->Interpret(context);

  ASSERT_TRUE(kResult.ok());
  EXPECT_EQ(*kResult, kDividend / kDivisor);
}

TEST_F(DivideExpressionSuite, ShouldReturnErrorForDivisionByZero) {
  Context context;
  const auto kDivExpr = std::make_unique<DivideExpression>(
      std::make_unique<NumberExpression>(kDividend),
      std::make_unique<NumberExpression>(0));

  const auto kResult = kDivExpr->Interpret(context);

  EXPECT_FALSE(kResult.ok());
  EXPECT_EQ(kResult.status().code(), absl::StatusCode::kInvalidArgument);
  EXPECT_NE(kResult.status().message().find("divide by zero"),
            std::string::npos);
}

TEST_F(DivideExpressionSuite, ShouldReturnErrorForNonIntegerResult) {
  Context context;
  const auto kDivExpr = std::make_unique<DivideExpression>(
      std::make_unique<NumberExpression>(kDividend),
      std::make_unique<NumberExpression>(kNonDivisor));

  const auto kResult = kDivExpr->Interpret(context);

  EXPECT_FALSE(kResult.ok());
  EXPECT_EQ(kResult.status().code(), absl::StatusCode::kInvalidArgument);
}

class ExpressionParserSuite : public ::testing::Test {
 protected:
  static constexpr std::int64_t kVarValue = 99;
  static constexpr std::int64_t kVarX = 10;
  static constexpr std::int64_t kVarY = 5;
  static constexpr std::int64_t kVarZ = 2;
  static constexpr std::int64_t kMyVarValue = 42;
};

TEST_F(ExpressionParserSuite, ShouldParseNumber) {
  Context context;

  const auto kParsedExpr = ExpressionParser::Parse("42");

  ASSERT_TRUE(kParsedExpr.ok());
  const auto kResult = (*kParsedExpr)->Interpret(context);
  ASSERT_TRUE(kResult.ok());
  EXPECT_EQ(*kResult, 42);
}

TEST_F(ExpressionParserSuite, ShouldParseNegativeNumber) {
  Context context;

  const auto kParsedExpr = ExpressionParser::Parse("-17");

  ASSERT_TRUE(kParsedExpr.ok());
  const auto kResult = (*kParsedExpr)->Interpret(context);
  ASSERT_TRUE(kResult.ok());
  EXPECT_EQ(*kResult, -17);
}

TEST_F(ExpressionParserSuite, ShouldParseVariable) {
  Context context;
  context.SetVariable("x", kVarValue);

  const auto kParsedExpr = ExpressionParser::Parse("x");

  ASSERT_TRUE(kParsedExpr.ok());
  const auto kResult = (*kParsedExpr)->Interpret(context);
  ASSERT_TRUE(kResult.ok());
  EXPECT_EQ(*kResult, kVarValue);
}

TEST_F(ExpressionParserSuite, ShouldParseAddition) {
  Context context;

  const auto kParsedExpr = ExpressionParser::Parse("10 + 5");

  ASSERT_TRUE(kParsedExpr.ok());
  const auto kResult = (*kParsedExpr)->Interpret(context);
  ASSERT_TRUE(kResult.ok());
  EXPECT_EQ(*kResult, 15);
}

TEST_F(ExpressionParserSuite, ShouldParseSubtraction) {
  Context context;

  const auto kParsedExpr = ExpressionParser::Parse("10 - 5");

  ASSERT_TRUE(kParsedExpr.ok());
  const auto kResult = (*kParsedExpr)->Interpret(context);
  ASSERT_TRUE(kResult.ok());
  EXPECT_EQ(*kResult, 5);
}

TEST_F(ExpressionParserSuite, ShouldParseMultiplication) {
  Context context;

  const auto kParsedExpr = ExpressionParser::Parse("10 * 5");

  ASSERT_TRUE(kParsedExpr.ok());
  const auto kResult = (*kParsedExpr)->Interpret(context);
  ASSERT_TRUE(kResult.ok());
  EXPECT_EQ(*kResult, 50);
}

TEST_F(ExpressionParserSuite, ShouldParseDivision) {
  Context context;

  const auto kParsedExpr = ExpressionParser::Parse("10 / 5");

  ASSERT_TRUE(kParsedExpr.ok());
  const auto kResult = (*kParsedExpr)->Interpret(context);
  ASSERT_TRUE(kResult.ok());
  EXPECT_EQ(*kResult, 2);
}

TEST_F(ExpressionParserSuite, ShouldRespectOperatorPrecedence) {
  Context context;

  const auto kParsedExpr = ExpressionParser::Parse("2 + 3 * 4");

  ASSERT_TRUE(kParsedExpr.ok());
  const auto kResult = (*kParsedExpr)->Interpret(context);
  ASSERT_TRUE(kResult.ok());
  EXPECT_EQ(*kResult, 14);
}

TEST_F(ExpressionParserSuite, ShouldRespectParentheses) {
  Context context;

  const auto kParsedExpr = ExpressionParser::Parse("(2 + 3) * 4");

  ASSERT_TRUE(kParsedExpr.ok());
  const auto kResult = (*kParsedExpr)->Interpret(context);
  ASSERT_TRUE(kResult.ok());
  EXPECT_EQ(*kResult, 20);
}

TEST_F(ExpressionParserSuite, ShouldParseNestedParentheses) {
  Context context;

  const auto kParsedExpr = ExpressionParser::Parse("((2 + 3) * (4 - 1))");

  ASSERT_TRUE(kParsedExpr.ok());
  const auto kResult = (*kParsedExpr)->Interpret(context);
  ASSERT_TRUE(kResult.ok());
  EXPECT_EQ(*kResult, 15);
}

TEST_F(ExpressionParserSuite, ShouldParseComplexExpression) {
  Context context;
  context.SetVariable("x", kVarX);
  context.SetVariable("y", kVarY);
  context.SetVariable("z", kVarZ);

  const auto kParsedExpr = ExpressionParser::Parse("(x + y) * z");

  ASSERT_TRUE(kParsedExpr.ok());
  const auto kResult = (*kParsedExpr)->Interpret(context);
  ASSERT_TRUE(kResult.ok());
  EXPECT_EQ(*kResult, (kVarX + kVarY) * kVarZ);
}

TEST_F(ExpressionParserSuite, ShouldHandleWhitespace) {
  Context context;

  const auto kParsedExpr = ExpressionParser::Parse("  10   +   5  ");

  ASSERT_TRUE(kParsedExpr.ok());
  const auto kResult = (*kParsedExpr)->Interpret(context);
  ASSERT_TRUE(kResult.ok());
  EXPECT_EQ(*kResult, 15);
}

TEST_F(ExpressionParserSuite, ShouldReturnErrorForEmptyExpression) {
  const auto kParsedExpr = ExpressionParser::Parse("");

  EXPECT_FALSE(kParsedExpr.ok());
  EXPECT_EQ(kParsedExpr.status().code(), absl::StatusCode::kInvalidArgument);
}

TEST_F(ExpressionParserSuite, ShouldReturnErrorForMissingOperand) {
  const auto kParsedExpr = ExpressionParser::Parse("10 +");

  EXPECT_FALSE(kParsedExpr.ok());
  EXPECT_EQ(kParsedExpr.status().code(), absl::StatusCode::kInvalidArgument);
}

TEST_F(ExpressionParserSuite, ShouldReturnErrorForMissingClosingParen) {
  const auto kParsedExpr = ExpressionParser::Parse("(10 + 5");

  EXPECT_FALSE(kParsedExpr.ok());
  EXPECT_EQ(kParsedExpr.status().code(), absl::StatusCode::kInvalidArgument);
  EXPECT_NE(kParsedExpr.status().message().find("parenthesis"), std::string::npos);
}

TEST_F(ExpressionParserSuite, ShouldReturnErrorForTrailingCharacters) {
  const auto kParsedExpr = ExpressionParser::Parse("10 + 5 @");

  EXPECT_FALSE(kParsedExpr.ok());
  EXPECT_EQ(kParsedExpr.status().code(), absl::StatusCode::kInvalidArgument);
}

TEST_F(ExpressionParserSuite, ShouldParseVariableWithUnderscore) {
  Context context;
  context.SetVariable("my_var", kMyVarValue);

  const auto kParsedExpr = ExpressionParser::Parse("my_var");

  ASSERT_TRUE(kParsedExpr.ok());
  const auto kResult = (*kParsedExpr)->Interpret(context);
  ASSERT_TRUE(kResult.ok());
  EXPECT_EQ(*kResult, kMyVarValue);
}

TEST_F(ExpressionParserSuite, ShouldParseChainedOperations) {
  Context context;

  const auto kParsedExpr = ExpressionParser::Parse("1 + 2 + 3 + 4");

  ASSERT_TRUE(kParsedExpr.ok());
  const auto kResult = (*kParsedExpr)->Interpret(context);
  ASSERT_TRUE(kResult.ok());
  EXPECT_EQ(*kResult, 10);
}

TEST_F(ExpressionParserSuite, ShouldParseChainedMultiplication) {
  Context context;

  const auto kParsedExpr = ExpressionParser::Parse("2 * 3 * 4");

  ASSERT_TRUE(kParsedExpr.ok());
  const auto kResult = (*kParsedExpr)->Interpret(context);
  ASSERT_TRUE(kResult.ok());
  EXPECT_EQ(*kResult, 24);
}

class OverflowSuite : public ::testing::Test {
 protected:
  static constexpr std::int64_t kMaxInt64 =
      std::numeric_limits<std::int64_t>::max();
  static constexpr std::int64_t kMinInt64 =
      std::numeric_limits<std::int64_t>::min();
};

TEST_F(OverflowSuite, ShouldReturnErrorForAdditionOverflow) {
  Context context;
  const auto kAddExpr = std::make_unique<AddExpression>(
      std::make_unique<NumberExpression>(kMaxInt64),
      std::make_unique<NumberExpression>(1));

  const auto kResult = kAddExpr->Interpret(context);

  EXPECT_FALSE(kResult.ok());
  EXPECT_EQ(kResult.status().code(), absl::StatusCode::kOutOfRange);
  EXPECT_NE(kResult.status().message().find("overflow"), std::string::npos);
}

TEST_F(OverflowSuite, ShouldReturnErrorForAdditionUnderflow) {
  Context context;
  const auto kAddExpr = std::make_unique<AddExpression>(
      std::make_unique<NumberExpression>(kMinInt64),
      std::make_unique<NumberExpression>(-1));

  const auto kResult = kAddExpr->Interpret(context);

  EXPECT_FALSE(kResult.ok());
  EXPECT_EQ(kResult.status().code(), absl::StatusCode::kOutOfRange);
  EXPECT_NE(kResult.status().message().find("underflow"), std::string::npos);
}

TEST_F(OverflowSuite, ShouldReturnErrorForSubtractionOverflow) {
  Context context;
  const auto kSubExpr = std::make_unique<SubtractExpression>(
      std::make_unique<NumberExpression>(kMaxInt64),
      std::make_unique<NumberExpression>(-1));

  const auto kResult = kSubExpr->Interpret(context);

  EXPECT_FALSE(kResult.ok());
  EXPECT_EQ(kResult.status().code(), absl::StatusCode::kOutOfRange);
  EXPECT_NE(kResult.status().message().find("overflow"), std::string::npos);
}

TEST_F(OverflowSuite, ShouldReturnErrorForSubtractionUnderflow) {
  Context context;
  const auto kSubExpr = std::make_unique<SubtractExpression>(
      std::make_unique<NumberExpression>(kMinInt64),
      std::make_unique<NumberExpression>(1));

  const auto kResult = kSubExpr->Interpret(context);

  EXPECT_FALSE(kResult.ok());
  EXPECT_EQ(kResult.status().code(), absl::StatusCode::kOutOfRange);
  EXPECT_NE(kResult.status().message().find("underflow"), std::string::npos);
}

TEST_F(OverflowSuite, ShouldReturnErrorForMultiplicationOverflowPositive) {
  Context context;
  const auto kMulExpr = std::make_unique<MultiplyExpression>(
      std::make_unique<NumberExpression>(kMaxInt64),
      std::make_unique<NumberExpression>(2));

  const auto kResult = kMulExpr->Interpret(context);

  EXPECT_FALSE(kResult.ok());
  EXPECT_EQ(kResult.status().code(), absl::StatusCode::kOutOfRange);
  EXPECT_NE(kResult.status().message().find("overflow"), std::string::npos);
}

TEST_F(OverflowSuite, ShouldReturnErrorForMultiplicationOverflowNegatives) {
  Context context;
  const auto kMulExpr = std::make_unique<MultiplyExpression>(
      std::make_unique<NumberExpression>(kMinInt64),
      std::make_unique<NumberExpression>(-2));

  const auto kResult = kMulExpr->Interpret(context);

  EXPECT_FALSE(kResult.ok());
  EXPECT_EQ(kResult.status().code(), absl::StatusCode::kOutOfRange);
  EXPECT_NE(kResult.status().message().find("overflow"), std::string::npos);
}

TEST_F(OverflowSuite, ShouldReturnErrorForMultiplicationUnderflow) {
  Context context;
  const auto kMulExpr = std::make_unique<MultiplyExpression>(
      std::make_unique<NumberExpression>(kMaxInt64),
      std::make_unique<NumberExpression>(-2));

  const auto kResult = kMulExpr->Interpret(context);

  EXPECT_FALSE(kResult.ok());
  EXPECT_EQ(kResult.status().code(), absl::StatusCode::kOutOfRange);
  EXPECT_NE(kResult.status().message().find("underflow"), std::string::npos);
}

TEST_F(OverflowSuite, ShouldReturnErrorForDivisionOverflow) {
  Context context;
  const auto kDivExpr = std::make_unique<DivideExpression>(
      std::make_unique<NumberExpression>(kMinInt64),
      std::make_unique<NumberExpression>(-1));

  const auto kResult = kDivExpr->Interpret(context);

  EXPECT_FALSE(kResult.ok());
  EXPECT_EQ(kResult.status().code(), absl::StatusCode::kOutOfRange);
  EXPECT_NE(kResult.status().message().find("overflow"), std::string::npos);
}

TEST_F(OverflowSuite, ShouldAllowMaxValueOperations) {
  Context context;

  // MAX + 0 should work
  const auto kAddExpr = std::make_unique<AddExpression>(
      std::make_unique<NumberExpression>(kMaxInt64),
      std::make_unique<NumberExpression>(0));
  const auto kAddResult = kAddExpr->Interpret(context);
  ASSERT_TRUE(kAddResult.ok());
  EXPECT_EQ(*kAddResult, kMaxInt64);

  // MAX * 1 should work
  const auto kMulExpr = std::make_unique<MultiplyExpression>(
      std::make_unique<NumberExpression>(kMaxInt64),
      std::make_unique<NumberExpression>(1));
  const auto kMulResult = kMulExpr->Interpret(context);
  ASSERT_TRUE(kMulResult.ok());
  EXPECT_EQ(*kMulResult, kMaxInt64);

  // MAX - 0 should work
  const auto kSubExpr = std::make_unique<SubtractExpression>(
      std::make_unique<NumberExpression>(kMaxInt64),
      std::make_unique<NumberExpression>(0));
  const auto kSubResult = kSubExpr->Interpret(context);
  ASSERT_TRUE(kSubResult.ok());
  EXPECT_EQ(*kSubResult, kMaxInt64);
}

TEST_F(OverflowSuite, ShouldAllowMinValueOperations) {
  Context context;

  // MIN + 0 should work
  const auto kAddExpr = std::make_unique<AddExpression>(
      std::make_unique<NumberExpression>(kMinInt64),
      std::make_unique<NumberExpression>(0));
  const auto kAddResult = kAddExpr->Interpret(context);
  ASSERT_TRUE(kAddResult.ok());
  EXPECT_EQ(*kAddResult, kMinInt64);

  // MIN * 1 should work
  const auto kMulExpr = std::make_unique<MultiplyExpression>(
      std::make_unique<NumberExpression>(kMinInt64),
      std::make_unique<NumberExpression>(1));
  const auto kMulResult = kMulExpr->Interpret(context);
  ASSERT_TRUE(kMulResult.ok());
  EXPECT_EQ(*kMulResult, kMinInt64);

  // MIN - 0 should work
  const auto kSubExpr = std::make_unique<SubtractExpression>(
      std::make_unique<NumberExpression>(kMinInt64),
      std::make_unique<NumberExpression>(0));
  const auto kSubResult = kSubExpr->Interpret(context);
  ASSERT_TRUE(kSubResult.ok());
  EXPECT_EQ(*kSubResult, kMinInt64);
}
