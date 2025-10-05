#ifndef GOF23_STD_OUT_CAPTURE_GUARD_H
#define GOF23_STD_OUT_CAPTURE_GUARD_H

#include <string>

#include <gtest/gtest.h>

class StdoutCaptureGuard {
 public:
  StdoutCaptureGuard() { Resume(); }

  ~StdoutCaptureGuard() { (void)Capture(); }

  StdoutCaptureGuard(const StdoutCaptureGuard&) = delete;

  StdoutCaptureGuard& operator=(const StdoutCaptureGuard&) = delete;

  void Resume() {
    if (collecting_) return;
    ::testing::internal::CaptureStdout();
    collecting_ = true;
  }

  std::string Capture() {
    if (!collecting_) return {};
    collecting_ = false;
    return ::testing::internal::GetCapturedStdout();
  }

 private:
  bool collecting_ = false;
};

#endif