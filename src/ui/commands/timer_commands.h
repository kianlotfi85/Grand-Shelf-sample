#pragma once
#include "services/timer_service.h"
#include "repositories/subject_repo.h"

namespace study_planner::ui {

class TimerCommands {
public:
  TimerCommands(TimerService& timer_service, SubjectRepo& subject_repo)
    : timer_service_(timer_service), subject_repo_(subject_repo) {}

  void show_menu();
  void start_session();
  void stop_session();
  void view_history();

private:
  TimerService& timer_service_;
  SubjectRepo& subject_repo_;
};

} // namespace study_planner::ui
