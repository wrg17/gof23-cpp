//
// Created by Will George on 10/20/25.
//

#include <iostream>

#include "adapter.h"

int main() {
  auto fix = std::make_shared<FixLib::Session>();
  auto rest = std::make_shared<RestLib::Client>();

  const Strategy s_fix{std::make_unique<FixRouter>(fix)};
  const Strategy s_rest{std::make_unique<RestRouter>(rest)};

  s_fix.Run();
  s_rest.Run();
}