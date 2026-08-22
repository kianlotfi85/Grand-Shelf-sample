#pragma once
#include "db/connection.h"
#include "models/study_set.h"
#include "models/set_session.h"
#include <vector>
#include <optional>

namespace study_planner {

class StudySetRepo {
public:
  explicit StudySetRepo(db::Connection& conn) : conn_(conn) {}

  int64_t create(const StudySet& set);
  std::optional<StudySet> find_by_id(int64_t id);
  std::vector<StudySet> find_all();
  void update(const StudySet& set);
  void remove(int64_t id);

  // Set sessions
  int64_t create_session(const SetSession& session);
  void update_session(const SetSession& session);
  std::vector<SetSession> find_sessions_by_set(int64_t set_id);

private:
  db::Connection& conn_;
};

} // namespace study_planner
