//
// Created by Will George on 9/16/25.
//

#include "singleton.h"

Singleton& Singleton::Instance() {
  static Singleton obj;
  return obj;
}

std::string Singleton::ToString() const {
  std::lock_guard guard(mutex_);
  return std::format("Singleton\n\tname: {}\n", name_);
}

Singleton::operator std::string() const { return this->ToString(); }

std::ostream& operator<<(std::ostream& out_stream, const Singleton& inst) {
  return out_stream << inst.ToString();
}

void Singleton::SetName(std::string name) {
  std::lock_guard guard(mutex_);
  name_ = std::move(name);
}
