#include "repositories/study_set_repo.h"
#include "db/statement.h"

namespace study_planner {

int64_t StudySetRepo::create(const StudySet& set) {
  db::Statement stmt(conn_.handle(),
    "INSERT INTO study_sets (name, description, work_duration_seconds, "
    "break_duration_seconds, long_break_seconds, sessions_before_long_break) "
    "VALUES (?, ?, ?, ?, ?, ?)");
  stmt.bind_text(1, set.name);
  stmt.bind_text(2, set.description);
  stmt.bind_int(3, set.work_duration_seconds);
  stmt.bind_int(4, set.break_duration_seconds);
  stmt.bind_int(5, set.long_break_seconds);
  stmt.bind_int(6, set.sessions_before_long_break);
  stmt.step();
  return conn_.last_insert_rowid();
}

std::optional<StudySet> StudySetRepo::find_by_id(int64_t id) {
  db::Statement stmt(conn_.handle(),
    "SELECT id, name, description, work_duration_seconds, break_duration_seconds, "
    "long_break_seconds, sessions_before_long_break, created_at FROM study_sets WHERE id = ?");
  stmt.bind_int(1, id);
  if (!stmt.step()) return std::nullopt;
  StudySet s;
  s.id = stmt.column_int64(0);
  s.name = stmt.column_text(1);
  s.description = stmt.column_text(2);
  s.work_duration_seconds = stmt.column_int64(3);
  s.break_duration_seconds = stmt.column_int64(4);
  s.long_break_seconds = stmt.column_int64(5);
  s.sessions_before_long_break = stmt.column_int64(6);
  s.created_at = stmt.column_text(7);
  return s;
}

std::vector<StudySet> StudySetRepo::find_all() {
  std::vector<StudySet> result;
  db::Statement stmt(conn_.handle(),
    "SELECT id, name, description, work_duration_seconds, break_duration_seconds, "
    "long_break_seconds, sessions_before_long_break, created_at FROM study_sets ORDER BY name");
  while (stmt.step()) {
    StudySet s;
    s.id = stmt.column_int64(0);
    s.name = stmt.column_text(1);
    s.description = stmt.column_text(2);
    s.work_duration_seconds = stmt.column_int64(3);
    s.break_duration_seconds = stmt.column_int64(4);
    s.long_break_seconds = stmt.column_int64(5);
    s.sessions_before_long_break = stmt.column_int64(6);
    s.created_at = stmt.column_text(7);
    result.push_back(std::move(s));
  }
  return result;
}

void StudySetRepo::update(const StudySet& set) {
  db::Statement stmt(conn_.handle(),
    "UPDATE study_sets SET name = ?, description = ?, work_duration_seconds = ?, "
    "break_duration_seconds = ?, long_break_seconds = ?, sessions_before_long_break = ? WHERE id = ?");
  stmt.bind_text(1, set.name);
  stmt.bind_text(2, set.description);
  stmt.bind_int(3, set.work_duration_seconds);
  stmt.bind_int(4, set.break_duration_seconds);
  stmt.bind_int(5, set.long_break_seconds);
  stmt.bind_int(6, set.sessions_before_long_break);
  stmt.bind_int(7, set.id);
  stmt.step();
}

void StudySetRepo::remove(int64_t id) {
  db::Statement stmt(conn_.handle(), "DELETE FROM study_sets WHERE id = ?");
  stmt.bind_int(1, id);
  stmt.step();
}

int64_t StudySetRepo::create_session(const SetSession& session) {
  db::Statement stmt(conn_.handle(),
    "INSERT INTO set_sessions (set_id, subject_id, session_type, start_time) VALUES (?, ?, ?, ?)");
  stmt.bind_int(1, session.set_id);
  if (session.subject_id) stmt.bind_int(2, *session.subject_id);
  else stmt.bind_null(2);
  stmt.bind_text(3, session.session_type);
  stmt.bind_text(4, session.start_time);
  stmt.step();
  return conn_.last_insert_rowid();
}

void StudySetRepo::update_session(const SetSession& session) {
  db::Statement stmt(conn_.handle(),
    "UPDATE set_sessions SET end_time = ? WHERE id = ?");
  if (session.end_time) stmt.bind_text(1, *session.end_time);
  else stmt.bind_null(1);
  stmt.bind_int(2, session.id);
  stmt.step();
}

std::vector<SetSession> StudySetRepo::find_sessions_by_set(int64_t set_id) {
  std::vector<SetSession> result;
  db::Statement stmt(conn_.handle(),
    "SELECT id, set_id, subject_id, session_type, start_time, end_time, created_at "
    "FROM set_sessions WHERE set_id = ? ORDER BY start_time DESC LIMIT 50");
  stmt.bind_int(1, set_id);
  while (stmt.step()) {
    SetSession s;
    s.id = stmt.column_int64(0);
    s.set_id = stmt.column_int64(1);
    if (!stmt.column_is_null(2)) s.subject_id = stmt.column_int64(2);
    s.session_type = stmt.column_text(3);
    s.start_time = stmt.column_text(4);
    if (!stmt.column_is_null(5)) s.end_time = stmt.column_text(5);
    s.created_at = stmt.column_text(6);
    result.push_back(std::move(s));
  }
  return result;
}

} // namespace study_planner
