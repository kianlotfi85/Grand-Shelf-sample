#pragma once
#include "repositories/session_repo.h"
#include "repositories/subject_repo.h"
#include "models/study_session.h"
#include <string>
#include <optional>
#include <functional>

namespace study_planner {

class TimerService {
public:
  using TickCallback = std::function<void(int64_t elapsed_seconds)>;

  TimerService(SessionRepo& session_repo, SubjectRepo& subject_repo)
    : session_repo_(session_repo), subject_repo_(subject_repo) {}

  bool has_running_session();
  std::optional<StudySession> get_running_session();
  int64_t start_session(int64_t subject_id, std::optional<int64_t> task_id, const std::string& notes);
  int64_t stop_session(int64_t session_id);
  std::vector<StudySession> get_session_history(int64_t subject_id);
  std::vector<StudySession> get_all_sessions();

  // Timer display helpers
  std::string format_duration(int64_t seconds);

private:
  SessionRepo& session_repo_;
  SubjectRepo& subject_repo_;
};

} // namespace study_planner
