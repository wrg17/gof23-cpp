//
// Created by Will George on 9/10/25.
//

#include <format>
#include <iostream>
#include <singleton.h>
#include <string>

int main() {
  std::cout << "01 — Singleton\n";
  std::cout << "~~~~~~~~~~~~~~\n";

  Singleton& obj = Singleton::Instance();
  obj.SetName("Will");

  std::cout << Singleton::Instance();

  Singleton& obj2 = Singleton::Instance();
  obj2.SetName("George");

  std::cout << std::format("\nobj=`\n{}`\nobj2=`\n{}`", obj.ToString(),
                           obj2.ToString());

  return 0;
}