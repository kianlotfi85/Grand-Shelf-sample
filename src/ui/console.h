#pragma once
#include "services/task_service.h"
#include "services/timer_service.h"
#include "services/analytics_service.h"
#include "services/planner_service.h"
#include "repositories/subject_repo.h"
#include "repositories/task_repo.h"
#include "repositories/session_repo.h"
#include "repositories/study_set_repo.h"
#include "repositories/plan_repo.h"
#include "db/connection.h"
#include "app/config.h"

namespace study_planner::ui {

class Console {
public:
  Console(db::Connection& conn, const Config& config);

  void run();

private:
  void show_main_menu();
  void manage_subjects();
  void manage_study_sets();
  void show_help();

  db::Connection& conn_;
  const Config& config_;

  // Repositories
  SubjectRepo subject_repo_;
  TaskRepo task_repo_;
  SessionRepo session_repo_;
  StudySetRepo study_set_repo_;
  PlanRepo plan_repo_;

  // Services
  TaskService task_service_;
  TimerService timer_service_;
  AnalyticsService analytics_service_;
  PlannerService planner_service_;

  bool running_ = true;
};

} // namespace study_planner::ui
