#pragma once
#include "services/planner_service.h"
#include "repositories/subject_repo.h"

namespace study_planner::ui {

class PlannerCommands {
public:
  PlannerCommands(PlannerService& planner_service, SubjectRepo& subject_repo)
    : planner_service_(planner_service), subject_repo_(subject_repo) {}

  void show_menu();
  void list_plans();
  void create_plan();
  void view_progress();
  void delete_plan();

private:
  PlannerService& planner_service_;
  SubjectRepo& subject_repo_;
};

} // namespace study_planner::ui
