#include "ui/commands/analytics_commands.h"
#include "ui/formatters/table_formatter.h"
#include <iostream>
#include <fmt/core.h>
#include <ctime>

namespace study_planner::ui {

std::string AnalyticsCommands::get_date_range(std::string& start, std::string& end) {
  fmt::print("  Start date (YYYY-MM-DD): ");
  std::getline(std::cin, start);
  fmt::print("  End date (YYYY-MM-DD): ");
  std::getline(std::cin, end);
  return start;
}

void AnalyticsCommands::show_menu() {
  while (true) {
    TableFormatter::print_header("Analytics");
    fmt::print("  1. Study time by subject\n");
    fmt::print("  2. Daily summary\n");
    fmt::print("  3. Total study time\n");
    fmt::print("  0. Back to main menu\n");
    fmt::print("\n  Choice: ");
    std::string input;
    std::getline(std::cin, input);
    if (input == "0") break;
    else if (input == "1") show_by_subject();
    else if (input == "2") show_daily_summary();
    else if (input == "3") show_total();
    else TableFormatter::print_error("Invalid choice");
  }
}

void AnalyticsCommands::show_by_subject() {
  std::string start, end;
  get_date_range(start, end);
  auto results = analytics_service_.total_by_subject(start, end + " 23:59:59");
  TableFormatter tf;
  tf.add_column("Subject", 20)
    .add_column("Total Time", 15)
    .add_column("Sessions", 10, true);
  for (auto& r : results) {
    tf.add_row({
      r.subject_name,
      analytics_service_.format_duration(r.total_seconds),
      std::to_string(r.session_count)
    });
  }
  tf.print();
}

void AnalyticsCommands::show_daily_summary() {
  std::string start, end;
  get_date_range(start, end);
  auto results = analytics_service_.daily_summary(start, end);
  TableFormatter tf;
  tf.add_column("Date", 14)
    .add_column("Study Time", 15);
  for (auto& d : results) {
    tf.add_row({
      d.date,
      analytics_service_.format_duration(d.total_seconds)
    });
  }
  tf.print();
}

void AnalyticsCommands::show_total() {
  std::string start, end;
  get_date_range(start, end);
  int64_t total = analytics_service_.total_study_time(start, end + " 23:59:59");
  TableFormatter::print_info(
    fmt::format("Total study time: {}", analytics_service_.format_duration(total)));
}

} // namespace study_planner::ui
