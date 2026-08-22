#include "services/analytics_service.h"
#include "db/statement.h"
#include <fmt/core.h>

namespace study_planner {

std::vector<SubjectAnalytics> AnalyticsService::total_by_subject(const std::string& start, const std::string& end) {
  std::vector<SubjectAnalytics> result;
  auto subjects = subject_repo_.find_all();
  for (auto& subj : subjects) {
    auto total = session_repo_.total_duration_by_subject(subj.id, start, end);
    if (total > 0) {
      SubjectAnalytics sa;
      sa.subject_id = subj.id;
      sa.subject_name = subj.name;
      sa.total_seconds = total;
      result.push_back(sa);
    }
  }
  return result;
}

std::vector<DailySummary> AnalyticsService::daily_summary(const std::string& start, const std::string& end) {
  std::vector<DailySummary> result;
  // Parse date range
  struct tm tm_start = {};
  strptime(start.c_str(), "%Y-%m-%d", &tm_start);
  struct tm tm_end = {};
  strptime(end.c_str(), "%Y-%m-%d", &tm_end);

  std::time_t t_start = std::mktime(&tm_start);
  std::time_t t_end = std::mktime(&tm_end);

  for (std::time_t t = t_start; t <= t_end; t += 86400) {
    char buf[16];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", std::localtime(&t));
    auto total = session_repo_.total_duration_by_date(buf);
    DailySummary ds;
    ds.date = buf;
    ds.total_seconds = total;
    result.push_back(ds);
  }
  return result;
}

int64_t AnalyticsService::total_study_time(const std::string& start, const std::string& end) {
  auto sql = fmt::format(
    "SELECT COALESCE(SUM(duration_seconds), 0) FROM study_sessions "
    "WHERE start_time >= '{}' AND start_time < '{}' AND end_time IS NOT NULL",
    start, end);
  auto& conn = session_repo_; // We need access to the connection
  // Use a simpler approach - iterate subjects
  int64_t total = 0;
  auto subjects = subject_repo_.find_all();
  for (auto& subj : subjects) {
    total += session_repo_.total_duration_by_subject(subj.id, start, end);
  }
  return total;
}

std::string AnalyticsService::format_duration(int64_t seconds) {
  int64_t hours = seconds / 3600;
  int64_t mins = (seconds % 3600) / 60;
  if (hours > 0) {
    return fmt::format("{}h {}m", hours, mins);
  }
  return fmt::format("{}m", mins);
}

} // namespace study_planner
