#include "ui/commands/timer_commands.h"
#include "ui/formatters/table_formatter.h"
#include <iostream>
#include <fmt/core.h>

namespace study_planner::ui {

void TimerCommands::show_menu() {
  while (true) {
    TableFormatter::print_header("Time Tracker");
    auto running = timer_service_.get_running_session();
    if (running) {
      auto subj = subject_repo_.find_by_id(running->subject_id);
      fmt::print(fmt::fg(fmt::color::yellow),
        "  ⏱  Session running: {} (started: {})\n\n",
        subj ? subj->name : "Unknown", running->start_time);
    }
    fmt::print("  1. Start study session\n");
    fmt::print("  2. Stop current session\n");
    fmt::print("  3. View session history\n");
    fmt::print("  0. Back to main menu\n");
    fmt::print("\n  Choice: ");
    std::string input;
    std::getline(std::cin, input);
    if (input == "0") break;
    else if (input == "1") start_session();
    else if (input == "2") stop_session();
    else if (input == "3") view_history();
    else TableFormatter::print_error("Invalid choice");
  }
}

void TimerCommands::start_session() {
  if (timer_service_.has_running_session()) {
    TableFormatter::print_error("A session is already running. Stop it first.");
    return;
  }

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
  fmt::print("  Task ID (optional, 0 for none): ");
  std::string tid; std::getline(std::cin, tid);
  fmt::print("  Notes (optional): ");
  std::string notes; std::getline(std::cin, notes);

  try {
    std::optional<int64_t> task_id;
    int64_t tid_val = std::stoll(tid);
    if (tid_val > 0) task_id = tid_val;
    int64_t id = timer_service_.start_session(std::stoll(sid), task_id, notes);
    TableFormatter::print_success(fmt::format("Session started (ID: {})", id));
    fmt::print("  Timer is running... press Enter to stop.\n");
  } catch (const std::exception& e) {
    TableFormatter::print_error(e.what());
  }
}

void TimerCommands::stop_session() {
  auto running = timer_service_.get_running_session();
  if (!running) {
    TableFormatter::print_error("No session is currently running.");
    return;
  }
  try {
    int64_t duration = timer_service_.stop_session(running->id);
    TableFormatter::print_success(
      fmt::format("Session stopped. Duration: {}", timer_service_.format_duration(duration)));
  } catch (const std::exception& e) {
    TableFormatter::print_error(e.what());
  }
}

void TimerCommands::view_history() {
  auto sessions = timer_service_.get_all_sessions();
  TableFormatter tf;
  tf.add_column("ID", 5, true)
    .add_column("Subject", 15)
    .add_column("Start", 20)
    .add_column("Duration", 12)
    .add_column("Notes", 20);
  for (auto& s : sessions) {
    auto subj = subject_repo_.find_by_id(s.subject_id);
    tf.add_row({
      std::to_string(s.id),
      subj ? subj->name : "?",
      s.start_time,
      timer_service_.format_duration(s.duration_seconds),
      s.notes
    });
  }
  tf.print();
  fmt::print("  Total: {} sessions\n", sessions.size());
}

} // namespace study_planner::ui
