#include "ui/console.h"
#include "ui/formatters/table_formatter.h"
#include "ui/commands/task_commands.h"
#include "ui/commands/timer_commands.h"
#include "ui/commands/analytics_commands.h"
#include "ui/commands/planner_commands.h"
#include <iostream>
#include <fmt/core.h>
#include <fmt/color.h>

namespace study_planner::ui {

Console::Console(db::Connection& conn, const Config& config)
  : conn_(conn)
  , config_(config)
  , subject_repo_(conn_)
  , task_repo_(conn_)
  , session_repo_(conn_)
  , study_set_repo_(conn_)
  , plan_repo_(conn_)
  , task_service_(task_repo_, subject_repo_)
  , timer_service_(session_repo_, subject_repo_)
  , analytics_service_(session_repo_, subject_repo_, task_repo_)
  , planner_service_(plan_repo_, session_repo_, subject_repo_)
{
}

void Console::run() {
  fmt::print(fmt::emphasis_val::bold | fmt::fg(fmt::color::cyan),
    "\n  ╔══════════════════════════════╗\n"
    "  ║     STUDY PLANNER v1.0      ║\n"
    "  ╚══════════════════════════════╝\n\n");

  while (running_) {
    show_main_menu();
  }

  fmt::print(fmt::fg(fmt::color::cyan), "\n  Goodbye! Keep studying! 📚\n\n");
}

void Console::show_main_menu() {
  fmt::print("  ┌─────────────────────────────┐\n");
  fmt::print("  │  1. Manage Subjects         │\n");
  fmt::print("  │  2. Manage Tasks            │\n");
  fmt::print("  │  3. Time Tracker            │\n");
  fmt::print("  │  4. Study Sets (Pomodoro)   │\n");
  fmt::print("  │  5. Analytics               │\n");
  fmt::print("  │  6. Planner                 │\n");
  fmt::print("  │  7. Help                    │\n");
  fmt::print("  │  0. Exit                    │\n");
  fmt::print("  └─────────────────────────────┘\n");
  fmt::print("\n  Choice: ");

  std::string input;
  std::getline(std::cin, input);

  if (input == "0") {
    running_ = false;
  } else if (input == "1") {
    manage_subjects();
  } else if (input == "2") {
    TaskCommands cmd(task_service_, subject_repo_);
    cmd.show_menu();
  } else if (input == "3") {
    TimerCommands cmd(timer_service_, subject_repo_);
    cmd.show_menu();
  } else if (input == "4") {
    manage_study_sets();
  } else if (input == "5") {
    AnalyticsCommands cmd(analytics_service_);
    cmd.show_menu();
  } else if (input == "6") {
    PlannerCommands cmd(planner_service_, subject_repo_);
    cmd.show_menu();
  } else if (input == "7") {
    show_help();
  } else {
    TableFormatter::print_error("Invalid choice. Try again.");
  }
}

void Console::manage_subjects() {
  while (true) {
    TableFormatter::print_header("Subjects");
    auto subjects = subject_repo_.find_all();
    if (subjects.empty()) {
      TableFormatter::print_info("No subjects yet. Create one to get started.");
    } else {
      TableFormatter tf;
      tf.add_column("ID", 5, true)
        .add_column("Name", 20)
        .add_column("Color", 10)
        .add_column("Description", 30);
      for (auto& s : subjects) {
        tf.add_row({
          std::to_string(s.id), s.name, s.color, s.description
        });
      }
      tf.print();
    }

    fmt::print("\n  1. Add subject\n");
    fmt::print("  2. Delete subject\n");
    fmt::print("  0. Back\n");
    fmt::print("\n  Choice: ");
    std::string input;
    std::getline(std::cin, input);

    if (input == "0") break;
    else if (input == "1") {
      Subject s;
      fmt::print("  Name: ");
      std::getline(std::cin, s.name);
      fmt::print("  Description (optional): ");
      std::getline(std::cin, s.description);
      fmt::print("  Color (optional, default #4A90D9): ");
      std::getline(std::cin, s.color);
      if (s.color.empty()) s.color = "#4A90D9";
      try {
        int64_t id = subject_repo_.create(s);
        TableFormatter::print_success(fmt::format("Subject '{}' created (ID: {})", s.name, id));
      } catch (const std::exception& e) {
        TableFormatter::print_error(e.what());
      }
    } else if (input == "2") {
      fmt::print("  Subject ID to delete: ");
      std::string sid; std::getline(std::cin, sid);
      try {
        subject_repo_.remove(std::stoll(sid));
        TableFormatter::print_success("Subject deleted");
      } catch (const std::exception& e) {
        TableFormatter::print_error(e.what());
      }
    } else {
      TableFormatter::print_error("Invalid choice");
    }
  }
}

void Console::manage_study_sets() {
  while (true) {
    TableFormatter::print_header("Study Sets (Pomodoro)");
    auto sets = study_set_repo_.find_all();

    if (sets.empty()) {
      // Create default Pomodoro set
      StudySet ps;
      ps.name = "Classic Pomodoro";
      ps.description = "25 min work, 5 min break, 15 min long break after 4 sessions";
      ps.work_duration_seconds = config_.defaults.work_duration_seconds;
      ps.break_duration_seconds = config_.defaults.break_duration_seconds;
      ps.long_break_seconds = config_.defaults.long_break_seconds;
      ps.sessions_before_long_break = config_.defaults.sessions_before_long_break;
      study_set_repo_.create(ps);
      sets = study_set_repo_.find_all();
    }

    TableFormatter tf;
    tf.add_column("ID", 5, true)
      .add_column("Name", 20)
      .add_column("Work", 8, true)
      .add_column("Break", 8, true)
      .add_column("Long Break", 12, true)
      .add_column("Sessions", 10, true);
    for (auto& s : sets) {
      tf.add_row({
        std::to_string(s.id), s.name,
        fmt::format("{}m", s.work_duration_seconds / 60),
        fmt::format("{}m", s.break_duration_seconds / 60),
        fmt::format("{}m", s.long_break_seconds / 60),
        std::to_string(s.sessions_before_long_break)
      });
    }
    tf.print();

    fmt::print("\n  1. Create custom set\n");
    fmt::print("  2. Run a set (simulated Pomodoro)\n");
    fmt::print("  0. Back\n");
    fmt::print("\n  Choice: ");
    std::string input;
    std::getline(std::cin, input);

    if (input == "0") break;
    else if (input == "1") {
      StudySet s;
      fmt::print("  Name: ");
      std::getline(std::cin, s.name);
      fmt::print("  Work duration (minutes): ");
      std::string w; std::getline(std::cin, w);
      s.work_duration_seconds = std::stoll(w) * 60;
      fmt::print("  Break duration (minutes): ");
      std::string b; std::getline(std::cin, b);
      s.break_duration_seconds = std::stoll(b) * 60;
      fmt::print("  Long break (minutes): ");
      std::string lb; std::getline(std::cin, lb);
      s.long_break_seconds = std::stoll(lb) * 60;
      fmt::print("  Sessions before long break: ");
      std::string sb; std::getline(std::cin, sb);
      s.sessions_before_long_break = std::stoll(sb);
      study_set_repo_.create(s);
      TableFormatter::print_success("Study set created");
    } else if (input == "2") {
      fmt::print("  Set ID: ");
      std::string sid; std::getline(std::cin, sid);
      auto set = study_set_repo_.find_by_id(std::stoll(sid));
      if (!set) {
        TableFormatter::print_error("Set not found");
        continue;
      }

      auto subjects = subject_repo_.find_all();
      if (subjects.empty()) {
        TableFormatter::print_error("No subjects. Create one first.");
        continue;
      }
      fmt::print("  Subjects:\n");
      for (auto& s : subjects) fmt::print("    {}: {}\n", s.id, s.name);
      fmt::print("  Subject ID: ");
      std::string subj_id; std::getline(std::cin, subj_id);

      fmt::print("  Number of sessions: ");
      std::string n; std::getline(std::cin, n);
      int num_sessions = std::stoi(n);

      fmt::print(fmt::fg(fmt::color::yellow),
        "\n  Starting Pomodoro: {} sessions of {}m work / {}m break\n\n",
        num_sessions, set->work_duration_seconds / 60, set->break_duration_seconds / 60);

      for (int i = 1; i <= num_sessions; i++) {
        fmt::print(fmt::fg(fmt::color::green), "  Session {}/{} — WORK ({} min)\n",
          i, num_sessions, set->work_duration_seconds / 60);

        // Log work session
        SetSession ss;
        ss.set_id = set->id;
        ss.subject_id = std::stoll(subj_id);
        ss.session_type = "work";
        ss.start_time = "now";
        study_set_repo_.create_session(ss);

        // Also create a study session
        StudySession study;
        study.subject_id = std::stoll(subj_id);
        study.start_time = "now";
        study.duration_seconds = set->work_duration_seconds;
        study.notes = fmt::format("Pomodoro session {}/{}", i, num_sessions);
        session_repo_.create(study);

        fmt::print("  Press Enter when work session is done...");
        std::string dummy; std::getline(std::cin, dummy);

        if (i < num_sessions) {
          bool is_long = (set->sessions_before_long_break > 0 &&
                         i % set->sessions_before_long_break == 0);
          int64_t break_sec = is_long ? set->long_break_seconds : set->break_duration_seconds;
          fmt::print(fmt::fg(fmt::color::blue), "  Break time! ({} min)\n", break_sec / 60);
          fmt::print("  Press Enter when break is done...");
          std::getline(std::cin, dummy);
        }
      }
      TableFormatter::print_success("Pomodoro set completed!");
    }
  }
}

void Console::show_help() {
  TableFormatter::print_header("Help");
  fmt::print("  Study Planner — a terminal-based study management tool.\n\n");
  fmt::print("  Quick Start:\n");
  fmt::print("    1. Create subjects (e.g., Math, Physics)\n");
  fmt::print("    2. Add tasks linked to subjects\n");
  fmt::print("    3. Start a study session with the timer\n");
  fmt::print("    4. Use Pomodoro sets for focused work intervals\n");
  fmt::print("    5. Check analytics to see your progress\n");
  fmt::print("    6. Create plans with daily/weekly goals\n\n");
  fmt::print("  Data is stored in: {}\n", config_.database.path);
  fmt::print("\n  Press Enter to continue...");
  std::string dummy; std::getline(std::cin, dummy);
}

} // namespace study_planner::ui
