//
// Created by Will George on 2/15/26.
//

#include <cstdint>
#include <iostream>

#include "interpreter.h"

namespace {
constexpr std::int64_t kVarX = 10;
constexpr std::int64_t kVarY = 5;
constexpr std::int64_t kVarZ = 2;
}  // namespace

int main() {
  Context context;

  context.SetVariable("x", kVarX);
  context.SetVariable("y", kVarY);
  context.SetVariable("z", kVarZ);

  std::cout << "=== Interpreter Pattern Demo ===\n";
  std::cout << "Variables: x = 10, y = 5, z = 2\n\n";

  EvaluateExpression("x + y", context);
  EvaluateExpression("x - y", context);
  EvaluateExpression("x * y", context);
  EvaluateExpression("x / z", context);

  EvaluateExpression("x + y * z", context);
  EvaluateExpression("x * y + z", context);

  EvaluateExpression("(x + y) * z", context);
  EvaluateExpression("x * (y + z)", context);

  EvaluateExpression("(x + y) * (x - y)", context);

  EvaluateExpression("100 + x * y", context);
  EvaluateExpression("(100 + x) * y", context);

  std::cout << "=== Error Cases ===\n\n";

  context.SetVariable("zero", 0);
  EvaluateExpression("x / zero", context);
  EvaluateExpression("y / z", context);
  EvaluateExpression("x + undefined", context);
  EvaluateExpression("x + ", context);
  EvaluateExpression("(x + y", context);

  return 0;
}
