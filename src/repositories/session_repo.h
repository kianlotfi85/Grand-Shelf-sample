#pragma once
#include "db/connection.h"
#include "models/study_session.h"
#include <vector>
#include <optional>
#include <string>

namespace study_planner {

class SessionRepo {
public:
  explicit SessionRepo(db::Connection& conn) : conn_(conn) {}

  int64_t create(const StudySession& session);
  std::optional<StudySession> find_by_id(int64_t id);
  std::optional<StudySession> find_running();
  std::vector<StudySession> find_by_subject(int64_t subject_id);
  std::vector<StudySession> find_by_date_range(const std::string& start, const std::string& end);
  std::vector<StudySession> find_all();
  void update(const StudySession& session);
  void remove(int64_t id);

  // Analytics helpers
  int64_t total_duration_by_subject(int64_t subject_id, const std::string& start, const std::string& end);
  int64_t total_duration_by_date(const std::string& date);

private:
  db::Connection& conn_;
};

} // namespace study_planner
