#pragma once
#include <string>
#include <vector>
#include <functional>
#include <fmt/core.h>
#include <fmt/color.h>

namespace study_planner::ui {

class TableFormatter {
public:
  struct Column {
    std::string header;
    int width;
    bool right_align = false;
  };

  TableFormatter& add_column(const std::string& header, int width, bool right_align = false);
  TableFormatter& add_row(const std::vector<std::string>& cells);
  void print();

  static void print_header(const std::string& title);
  static void print_success(const std::string& msg);
  static void print_error(const std::string& msg);
  static void print_warning(const std::string& msg);
  static void print_info(const std::string& msg);

private:
  std::vector<Column> columns_;
  std::vector<std::vector<std::string>> rows_;
};

} // namespace study_planner::ui
