#include "ui/formatters/table_formatter.h"
#include <iostream>
#include <iomanip>

namespace study_planner::ui {

TableFormatter& TableFormatter::add_column(const std::string& header, int width, bool right_align) {
  columns_.push_back({header, width, right_align});
  return *this;
}

TableFormatter& TableFormatter::add_row(const std::vector<std::string>& cells) {
  rows_.push_back(cells);
  return *this;
}

void TableFormatter::print() {
  // Print separator line
  std::string sep = "+";
  for (auto& col : columns_) {
    sep += std::string(col.width + 2, '-') + "+";
  }
  std::cout << sep << "\n";

  // Print header
  std::cout << "|";
  for (size_t i = 0; i < columns_.size(); i++) {
    std::cout << " " << std::setw(columns_[i].width);
    if (columns_[i].right_align)
      std::cout << std::right;
    else
      std::cout << std::left;
    std::cout << columns_[i].header << " |";
  }
  std::cout << "\n" << sep << "\n";

  // Print rows
  for (auto& row : rows_) {
    std::cout << "|";
    for (size_t i = 0; i < columns_.size() && i < row.size(); i++) {
      std::cout << " " << std::setw(columns_[i].width);
      if (columns_[i].right_align)
        std::cout << std::right;
      else
        std::cout << std::left;
      std::string cell = row[i];
      if (static_cast<int>(cell.size()) > columns_[i].width) {
        cell = cell.substr(0, columns_[i].width - 3) + "...";
      }
      std::cout << cell << " |";
    }
    std::cout << "\n";
  }
  std::cout << sep << "\n";
}

void TableFormatter::print_header(const std::string& title) {
  fmt::print(fmt::emphasis_val::bold | fmt::fg(fmt::color::cyan), "\n=== {} ===\n\n", title);
}

void TableFormatter::print_success(const std::string& msg) {
  fmt::print(fmt::fg(fmt::color::green), "  ✓ {}\n", msg);
}

void TableFormatter::print_error(const std::string& msg) {
  fmt::print(fmt::fg(fmt::color::red), "  ✗ Error: {}\n", msg);
}

void TableFormatter::print_warning(const std::string& msg) {
  fmt::print(fmt::fg(fmt::color::yellow), "  ⚠ {}\n", msg);
}

void TableFormatter::print_info(const std::string& msg) {
  fmt::print(fmt::fg(fmt::color::white), "  {}\n", msg);
}

} // namespace study_planner::ui
