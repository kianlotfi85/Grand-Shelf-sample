#pragma once
#include "repositories/session_repo.h"
#include "repositories/subject_repo.h"
#include "repositories/task_repo.h"
#include <string>
#include <vector>
#include <map>

namespace study_planner {

struct SubjectAnalytics {
  int64_t subject_id;
  std::string subject_name;
  int64_t total_seconds;
  int64_t session_count;
};

struct DailySummary {
  std::string date;
  int64_t total_seconds;
};

struct TimeWindow {
  std::string start;
  std::string end;
};

class AnalyticsService {
public:
  AnalyticsService(SessionRepo& session_repo, SubjectRepo& subject_repo, TaskRepo& task_repo)
    : session_repo_(session_repo), subject_repo_(subject_repo), task_repo_(task_repo) {}

  std::vector<SubjectAnalytics> total_by_subject(const std::string& start, const std::string& end);
  std::vector<DailySummary> daily_summary(const std::string& start, const std::string& end);
  int64_t total_study_time(const std::string& start, const std::string& end);
  std::string format_duration(int64_t seconds);

private:
  SessionRepo& session_repo_;
  SubjectRepo& subject_repo_;
  TaskRepo& task_repo_;
};

} // namespace study_planner
