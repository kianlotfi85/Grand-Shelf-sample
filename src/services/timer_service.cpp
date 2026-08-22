#include "services/timer_service.h"
#include <fmt/core.h>
#include <ctime>
#include <stdexcept>

namespace study_planner {

bool TimerService::has_running_session() {
  return session_repo_.find_running().has_value();
}

std::optional<StudySession> TimerService::get_running_session() {
  return session_repo_.find_running();
}

int64_t TimerService::start_session(int64_t subject_id, std::optional<int64_t> task_id, const std::string& notes) {
  if (has_running_session()) {
    throw std::runtime_error("A study session is already running. Stop it first.");
  }

  std::time_t now = std::time(nullptr);
  char buf[32];
  std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

  StudySession session;
  session.subject_id = subject_id;
  session.task_id = task_id;
  session.start_time = buf;
  session.notes = notes;
  session.duration_seconds = 0;

  return session_repo_.create(session);
}

int64_t TimerService::stop_session(int64_t session_id) {
  auto session = session_repo_.find_by_id(session_id);
  if (!session) {
    throw std::runtime_error(fmt::format("Session {} not found", session_id));
  }
  if (session->end_time.has_value()) {
    throw std::runtime_error("Session is already stopped");
  }

  std::time_t now = std::time(nullptr);
  char buf[32];
  std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
  session->end_time = buf;

  // Calculate duration
  struct tm start_tm = {};
  strptime(session->start_time.c_str(), "%Y-%m-%d %H:%M:%S", &start_tm);
  std::time_t start_t = std::mktime(&start_tm);
  session->duration_seconds = static_cast<int64_t>(std::difftime(now, start_t));

  session_repo_.update(*session);
  return session->duration_seconds;
}

std::vector<StudySession> TimerService::get_session_history(int64_t subject_id) {
  return session_repo_.find_by_subject(subject_id);
}

std::vector<StudySession> TimerService::get_all_sessions() {
  return session_repo_.find_all();
}

std::string TimerService::format_duration(int64_t seconds) {
  int64_t hours = seconds / 3600;
  int64_t mins = (seconds % 3600) / 60;
  int64_t secs = seconds % 60;
  if (hours > 0) {
    return fmt::format("{}h {}m {}s", hours, mins, secs);
  }
  return fmt::format("{}m {}s", mins, secs);
}

} // namespace study_planner
