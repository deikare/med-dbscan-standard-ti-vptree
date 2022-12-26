#pragma once

#include <chrono>
#include <string>
#include <vector>

typedef std::chrono::time_point<std::chrono::high_resolution_clock>
    timestamp_t;

enum class TIME_PRESCALER { MILLI, MICRO };

class TimeAnalyzer {
  const std::size_t SPACING_VAL = 30;
  TIME_PRESCALER prescaler_;

  timestamp_t startTime_;
  std::vector<std::string> taskNames_;
  std::vector<int64_t> times_;
  void printTimestamp_(std::string task_name, int64_t duration);

 public:
  TimeAnalyzer(TIME_PRESCALER prescaler = TIME_PRESCALER::MILLI) {
    startTime_ = std::chrono::high_resolution_clock::now();
    prescaler_ = prescaler;
  }

  /** Method saves task name and its finished time and optionally prints the
   * info
   * @param taks_name name of the task
   * @param print if true information will be printed straight away
   */
  void saveDurationNow(std::string task_name, bool print = false);

  /** Method prints final timing report from data collected from separate tasks
   */
  void printRaport();
};
