#include "services/task_service.h"
#include <fmt/core.h>
#include <ctime>

namespace study_planner {

int64_t TaskService::create_task(const std::string& title, int64_t subject_id,
                                  const std::string& description, int priority,
                                  const std::string& due_date, int estimated_minutes) {
  Task task;
  task.subject_id = subject_id;
  task.title = title;
  task.description = description;
  task.priority = priority;
  task.status = "todo";
  task.estimated_minutes = estimated_minutes;
  if (!due_date.empty()) task.due_date = due_date;
  return task_repo_.create(task);
}

std::optional<Task> TaskService::get_task(int64_t id) {
  return task_repo_.find_by_id(id);
}

std::vector<Task> TaskService::get_tasks_by_subject(int64_t subject_id) {
  return task_repo_.find_by_subject(subject_id);
}

std::vector<Task> TaskService::get_all_tasks() {
  return task_repo_.find_all();
}

std::vector<Task> TaskService::get_tasks_by_status(const std::string& status) {
  TaskFilter filter;
  filter.status = status;
  return task_repo_.find_by_filter(filter);
}

void TaskService::update_status(int64_t id, const std::string& new_status) {
  auto task = task_repo_.find_by_id(id);
  if (!task) {
    throw std::runtime_error(fmt::format("Task {} not found", id));
  }
  task->status = new_status;
  if (new_status == "completed") {
    std::time_t now = std::time(nullptr);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    task->completed_at = buf;
  }
  task_repo_.update(*task);
}

void TaskService::update_task(const Task& task) {
  task_repo_.update(task);
}

void TaskService::delete_task(int64_t id) {
  task_repo_.remove(id);
}

} // namespace study_planner
