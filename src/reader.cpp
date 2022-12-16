#include "reader.hpp"

#include <iostream>

auto CSVReader::stripLine_(std::string line) -> std::vector<double> {
  std::stringstream ss(line);
  std::vector<double> values;
  while (ss.good()) {
    std::string substr;
    getline(ss, substr, ',');
    values.push_back(stod(substr));
  }
  return values;
}

auto CSVReader::getData(std::string filename, bool has_class_column,
                        int skip_lines)
    -> std::pair<std::vector<std::vector<double>>, std::vector<double>> {
  std::ifstream csv_file(filename);
  std::string processed_line;
  std::vector<double> processed_row;

  for (int i = 0; i < skip_lines; ++i) {
    // this loop is skipping first lines (metadata) of csv file
    getline(csv_file, processed_line);
  }

  // data structures for saving data values and classes (if needed)
  std::vector<std::vector<double>> data;
  std::vector<double> classes;

  // reading reamaining points
  while (getline(csv_file, processed_line)) {
    processed_row = stripLine_(processed_line);
    if (has_class_column) {
      // separate values from class data
      classes.push_back(processed_row.back());
      processed_row.pop_back();
    }
    data.push_back(processed_row);
  }
  return std::make_pair<std::vector<std::vector<double>>, std::vector<double>>(
      std::move(data), std::move(classes));
}
