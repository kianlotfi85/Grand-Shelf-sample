#pragma once
#include "repositories/task_repo.h"
#include "repositories/subject_repo.h"
#include "models/task.h"
#include "models/subject.h"
#include <vector>
#include <optional>
#include <string>

namespace study_planner {

class TaskService {
public:
  TaskService(TaskRepo& task_repo, SubjectRepo& subject_repo)
    : task_repo_(task_repo), subject_repo_(subject_repo) {}

  int64_t create_task(const std::string& title, int64_t subject_id,
                      const std::string& description, int priority,
                      const std::string& due_date, int estimated_minutes);
  std::optional<Task> get_task(int64_t id);
  std::vector<Task> get_tasks_by_subject(int64_t subject_id);
  std::vector<Task> get_all_tasks();
  std::vector<Task> get_tasks_by_status(const std::string& status);
  void update_status(int64_t id, const std::string& new_status);
  void update_task(const Task& task);
  void delete_task(int64_t id);

private:
  TaskRepo& task_repo_;
  SubjectRepo& subject_repo_;
};

} // namespace study_planner
