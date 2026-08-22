#pragma once
#include "services/analytics_service.h"
#include <string>

namespace study_planner::ui {

class AnalyticsCommands {
public:
  explicit AnalyticsCommands(AnalyticsService& analytics_service)
    : analytics_service_(analytics_service) {}

  void show_menu();
  void show_by_subject();
  void show_daily_summary();
  void show_total();

private:
  AnalyticsService& analytics_service_;
  std::string get_date_range(std::string& start, std::string& end);
};

} // namespace study_planner::ui
