#include "analyzer.hpp"

#include <iostream>

void TimeAnalyzer::saveDurationNow(std::string task_name, bool print) {
  auto now_time = std::chrono::high_resolution_clock::now();
  int64_t duration;
  switch (prescaler_) {
    case TIME_PRESCALER::MICRO:
      duration = std::chrono::duration_cast<std::chrono::microseconds>(
                     now_time - startTime_)
                     .count();
      break;
    default:
      duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                     now_time - startTime_)
                     .count();
  }
  if (times_.size() > 0) {
    // removing time accumulated by previous tasks
    duration -= times_.back();
  }
  taskNames_.push_back(task_name);
  times_.push_back(duration);
  if (print) {
    printTimestamp_(task_name, duration);
  }
}

void TimeAnalyzer::printTimestamp_(std::string task_name, int64_t duration,
                                   std::size_t dash_length) {
  std::cout << task_name;
  for (std::size_t i = 0; i < SPACING_VAL - task_name.size(); ++i) {
    std::cout << " ";
  }
  std::cout << "| ";
  std::cout << duration << "\n";
  for (std::size_t i = 0; i < dash_length; ++i) {
    std::cout << "-";
  }
  std::cout << "\n";
}

void TimeAnalyzer::printRaport() {
  std::cout << "\nTimes of execution (by tasks): \n";
  std::string first_col_header("task name");
  std::string second_col_header("execution time");

  // choosing unit prefix
  switch (prescaler_) {
    case TIME_PRESCALER::MICRO:
      second_col_header += std::string(" [us]");
      break;
    default:
      second_col_header += std::string(" [ms]");
  }
  // DASH_LENGTH is universal width of dashes in a row
  const std::size_t DASH_LENGTH = SPACING_VAL + 2 + second_col_header.size();
  for (std::size_t i = 0; i < DASH_LENGTH; ++i) {
    std::cout << "-";
  }
  std::cout << "\n";
  std::cout << first_col_header;

  // filling rest of cell with spaces
  for (std::size_t i = 0; i < SPACING_VAL - first_col_header.size(); ++i) {
    std::cout << " ";
  }
  std::cout << "| ";
  std::cout << second_col_header << "\n";
  for (std::size_t i = 0; i < DASH_LENGTH; ++i) {
    std::cout << "-";
  }
  std::cout << "\n";

  // printing names of tasks and the times
  for (std::size_t i = 0; i < taskNames_.size(); ++i) {
    printTimestamp_(taskNames_.at(i), times_.at(i), DASH_LENGTH);
  }
  for (std::size_t i = 0; i < DASH_LENGTH; ++i) {
    std::cout << "-";
  }

  // printing summary
  std::cout << "\n";
  const std::string summary = "Total time of execution";
  std::cout << summary;
  for (std::size_t i = 0; i < SPACING_VAL - summary.size(); ++i) {
    std::cout << " ";
  }
  std::cout << "| ";
  int sum = 0;
  for (std::size_t i = 0; i < times_.size(); ++i) {
    sum += times_.at(i);
  }
  std::cout << sum << "\n";
}
