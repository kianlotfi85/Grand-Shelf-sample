#include "ui/commands/planner_commands.h"
#include "ui/formatters/table_formatter.h"
#include <iostream>
#include <fmt/core.h>

namespace study_planner::ui {

void PlannerCommands::show_menu() {
  while (true) {
    TableFormatter::print_header("Planner");
    fmt::print("  1. List all plans\n");
    fmt::print("  2. Create new plan\n");
    fmt::print("  3. View progress for a date\n");
    fmt::print("  4. Delete plan\n");
    fmt::print("  0. Back to main menu\n");
    fmt::print("\n  Choice: ");
    std::string input;
    std::getline(std::cin, input);
    if (input == "0") break;
    else if (input == "1") list_plans();
    else if (input == "2") create_plan();
    else if (input == "3") view_progress();
    else if (input == "4") delete_plan();
    else TableFormatter::print_error("Invalid choice");
  }
}

void PlannerCommands::list_plans() {
  auto plans = planner_service_.get_all_plans();
  TableFormatter tf;
  tf.add_column("ID", 5, true)
    .add_column("Subject", 15)
    .add_column("Type", 10)
    .add_column("Date", 12)
    .add_column("Target Min", 10, true)
    .add_column("Done", 6);
  for (auto& p : plans) {
    auto subj = subject_repo_.find_by_id(p.subject_id);
    tf.add_row({
      std::to_string(p.id),
      subj ? subj->name : "?",
      p.plan_type,
      p.target_date,
      std::to_string(p.target_minutes),
      p.is_completed ? "Yes" : "No"
    });
  }
  tf.print();
}

void PlannerCommands::create_plan() {
  auto subjects = subject_repo_.find_all();
  if (subjects.empty()) {
    TableFormatter::print_error("No subjects. Create one first.");
    return;
  }
  fmt::print("\n  Subjects:\n");
  for (auto& s : subjects) {
    fmt::print("    {}: {}\n", s.id, s.name);
  }
  fmt::print("  Subject ID: ");
  std::string sid; std::getline(std::cin, sid);
  fmt::print("  Plan type (daily/weekly/monthly): ");
  std::string type; std::getline(std::cin, type);
  fmt::print("  Target date (YYYY-MM-DD): ");
  std::string date; std::getline(std::cin, date);
  fmt::print("  Target minutes: ");
  std::string mins; std::getline(std::cin, mins);
  fmt::print("  Description (optional): ");
  std::string desc; std::getline(std::cin, desc);

  try {
    int64_t id = planner_service_.create_plan(
      std::stoll(sid), type, date, std::stoll(mins), desc);
    TableFormatter::print_success(fmt::format("Plan created with ID {}", id));
  } catch (const std::exception& e) {
    TableFormatter::print_error(e.what());
  }
}

void PlannerCommands::view_progress() {
  fmt::print("  Date (YYYY-MM-DD): ");
  std::string date; std::getline(std::cin, date);
  auto progress = planner_service_.get_progress(date);
  TableFormatter tf;
  tf.add_column("Subject", 15)
    .add_column("Type", 8)
    .add_column("Target", 10, true)
    .add_column("Actual", 10, true)
    .add_column("%", 7, true);
  for (auto& p : progress) {
    tf.add_row({
      p.subject_name,
      p.plan.plan_type,
      fmt::format("{} min", p.plan.target_minutes),
      fmt::format("{} min", p.actual_minutes),
      fmt::format("{:.0f}%", p.completion_pct)
    });
  }
  tf.print();
}

void PlannerCommands::delete_plan() {
  fmt::print("  Plan ID to delete: ");
  std::string pid; std::getline(std::cin, pid);
  try {
    planner_service_.delete_plan(std::stoll(pid));
    TableFormatter::print_success("Plan deleted");
  } catch (const std::exception& e) {
    TableFormatter::print_error(e.what());
  }
}

} // namespace study_planner::ui
