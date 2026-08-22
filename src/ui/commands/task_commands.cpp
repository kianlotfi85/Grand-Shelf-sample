#include "ui/commands/task_commands.h"
#include "ui/formatters/table_formatter.h"
#include <iostream>
#include <fmt/core.h>

namespace study_planner::ui {

void TaskCommands::show_menu() {
  while (true) {
    TableFormatter::print_header("Task Manager");
    fmt::print("  1. List all tasks\n");
    fmt::print("  2. Add new task\n");
    fmt::print("  3. Update task status\n");
    fmt::print("  4. Delete task\n");
    fmt::print("  0. Back to main menu\n");
    fmt::print("\n  Choice: ");
    std::string input;
    std::getline(std::cin, input);
    if (input == "0") break;
    else if (input == "1") list_tasks();
    else if (input == "2") add_task();
    else if (input == "3") update_task_status();
    else if (input == "4") delete_task();
    else TableFormatter::print_error("Invalid choice");
  }
}

void TaskCommands::list_tasks() {
  auto tasks = task_service_.get_all_tasks();
  TableFormatter tf;
  tf.add_column("ID", 5, true)
    .add_column("Subject", 15)
    .add_column("Title", 25)
    .add_column("Status", 12)
    .add_column("Priority", 8)
    .add_column("Est. Min", 8, true);
  for (auto& t : tasks) {
    auto subj = subject_repo_.find_by_id(t.subject_id);
    std::string prio = t.priority == 2 ? "High" : t.priority == 1 ? "Med" : "Low";
    tf.add_row({
      std::to_string(t.id),
      subj ? subj->name : "?",
      t.title,
      t.status,
      prio,
      std::to_string(t.estimated_minutes)
    });
  }
  tf.print();
  fmt::print("  Total: {} tasks\n", tasks.size());
}

void TaskCommands::add_task() {
  auto subjects = subject_repo_.find_all();
  if (subjects.empty()) {
    TableFormatter::print_error("No subjects exist. Create a subject first.");
    return;
  }

  fmt::print("\n  Subjects:\n");
  for (auto& s : subjects) {
    fmt::print("    {}: {}\n", s.id, s.name);
  }

  fmt::print("  Subject ID: ");
  std::string sid; std::getline(std::cin, sid);
  fmt::print("  Title: ");
  std::string title; std::getline(std::cin, title);
  fmt::print("  Description (optional): ");
  std::string desc; std::getline(std::cin, desc);
  fmt::print("  Priority (0=low, 1=med, 2=high): ");
  std::string prio; std::getline(std::cin, prio);
  fmt::print("  Due date (YYYY-MM-DD, optional): ");
  std::string due; std::getline(std::cin, due);
  fmt::print("  Estimated minutes: ");
  std::string est; std::getline(std::cin, est);

  try {
    int64_t id = task_service_.create_task(
      title, std::stoll(sid), desc,
      prio.empty() ? 0 : std::stoi(prio),
      due, est.empty() ? 0 : std::stoi(est));
    TableFormatter::print_success(fmt::format("Task created with ID {}", id));
  } catch (const std::exception& e) {
    TableFormatter::print_error(e.what());
  }
}

void TaskCommands::update_task_status() {
  fmt::print("  Task ID: ");
  std::string tid; std::getline(std::cin, tid);
  fmt::print("  New status (todo/in_progress/completed): ");
  std::string status; std::getline(std::cin, status);

  try {
    task_service_.update_status(std::stoll(tid), status);
    TableFormatter::print_success("Task status updated");
  } catch (const std::exception& e) {
    TableFormatter::print_error(e.what());
  }
}

void TaskCommands::delete_task() {
  fmt::print("  Task ID to delete: ");
  std::string tid; std::getline(std::cin, tid);
  try {
    task_service_.delete_task(std::stoll(tid));
    TableFormatter::print_success("Task deleted");
  } catch (const std::exception& e) {
    TableFormatter::print_error(e.what());
  }
}

} // namespace study_planner::ui
