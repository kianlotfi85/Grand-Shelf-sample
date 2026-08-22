#pragma once
#include "services/task_service.h"
#include "repositories/subject_repo.h"

namespace study_planner::ui {

class TaskCommands {
public:
  TaskCommands(TaskService& task_service, SubjectRepo& subject_repo)
    : task_service_(task_service), subject_repo_(subject_repo) {}

  void show_menu();
  void list_tasks();
  void add_task();
  void update_task_status();
  void delete_task();

private:
  TaskService& task_service_;
  SubjectRepo& subject_repo_;
};

} // namespace study_planner::ui
