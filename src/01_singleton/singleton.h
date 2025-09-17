//
// Created by Will George on 9/16/25.
//

#ifndef GOF23_SINGLETON_H
#define GOF23_SINGLETON_H

#include <mutex>
#include <ostream>
#include <string>

class Singleton {
 public:
  static Singleton& Instance();

  Singleton(const Singleton&) = delete;

  Singleton& operator=(const Singleton&) = delete;

  std::string ToString() const;

  explicit operator std::string() const;

  void SetName(std::string name);

  friend std::ostream& operator<<(std::ostream& out_stream,
                                  const Singleton& inst);

 protected:
  Singleton() = default;

  ~Singleton() = default;

 private:
  mutable std::mutex mutex_;
  std::string name_;
};

#endif  // GOF23_SINGLETON_H
