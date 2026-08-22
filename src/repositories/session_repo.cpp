#include "repositories/session_repo.h"
#include "db/statement.h"
#include <fmt/core.h>

namespace study_planner {

static StudySession parse_session_row(db::Statement& stmt) {
  StudySession s;
  s.id = stmt.column_int64(0);
  s.subject_id = stmt.column_int64(1);
  if (!stmt.column_is_null(2)) s.task_id = stmt.column_int64(2);
  s.start_time = stmt.column_text(3);
  if (!stmt.column_is_null(4)) s.end_time = stmt.column_text(4);
  s.duration_seconds = stmt.column_int64(5);
  s.notes = stmt.column_text(6);
  s.created_at = stmt.column_text(7);
  return s;
}

int64_t SessionRepo::create(const StudySession& session) {
  db::Statement stmt(conn_.handle(),
    "INSERT INTO study_sessions (subject_id, task_id, start_time, end_time, duration_seconds, notes) "
    "VALUES (?, ?, ?, ?, ?, ?)");
  stmt.bind_int(1, session.subject_id);
  if (session.task_id) stmt.bind_int(2, *session.task_id);
  else stmt.bind_null(2);
  stmt.bind_text(3, session.start_time);
  if (session.end_time) stmt.bind_text(4, *session.end_time);
  else stmt.bind_null(4);
  stmt.bind_int(5, session.duration_seconds);
  stmt.bind_text(6, session.notes);
  stmt.step();
  return conn_.last_insert_rowid();
}

std::optional<StudySession> SessionRepo::find_by_id(int64_t id) {
  db::Statement stmt(conn_.handle(),
    "SELECT id, subject_id, task_id, start_time, end_time, duration_seconds, notes, created_at "
    "FROM study_sessions WHERE id = ?");
  stmt.bind_int(1, id);
  if (!stmt.step()) return std::nullopt;
  return parse_session_row(stmt);
}

std::optional<StudySession> SessionRepo::find_running() {
  db::Statement stmt(conn_.handle(),
    "SELECT id, subject_id, task_id, start_time, end_time, duration_seconds, notes, created_at "
    "FROM study_sessions WHERE end_time IS NULL LIMIT 1");
  if (!stmt.step()) return std::nullopt;
  return parse_session_row(stmt);
}

std::vector<StudySession> SessionRepo::find_by_subject(int64_t subject_id) {
  std::vector<StudySession> result;
  db::Statement stmt(conn_.handle(),
    "SELECT id, subject_id, task_id, start_time, end_time, duration_seconds, notes, created_at "
    "FROM study_sessions WHERE subject_id = ? ORDER BY start_time DESC LIMIT 100");
  stmt.bind_int(1, subject_id);
  while (stmt.step()) {
    result.push_back(parse_session_row(stmt));
  }
  return result;
}

std::vector<StudySession> SessionRepo::find_by_date_range(const std::string& start, const std::string& end) {
  std::vector<StudySession> result;
  auto sql = fmt::format(
    "SELECT id, subject_id, task_id, start_time, end_time, duration_seconds, notes, created_at "
    "FROM study_sessions WHERE start_time >= '{}' AND start_time < '{}' ORDER BY start_time DESC",
    start, end);
  db::Statement stmt(conn_.handle(), sql);
  while (stmt.step()) {
    result.push_back(parse_session_row(stmt));
  }
  return result;
}

std::vector<StudySession> SessionRepo::find_all() {
  std::vector<StudySession> result;
  db::Statement stmt(conn_.handle(),
    "SELECT id, subject_id, task_id, start_time, end_time, duration_seconds, notes, created_at "
    "FROM study_sessions ORDER BY start_time DESC LIMIT 200");
  while (stmt.step()) {
    result.push_back(parse_session_row(stmt));
  }
  return result;
}

void SessionRepo::update(const StudySession& session) {
  db::Statement stmt(conn_.handle(),
    "UPDATE study_sessions SET subject_id = ?, task_id = ?, start_time = ?, "
    "end_time = ?, duration_seconds = ?, notes = ? WHERE id = ?");
  stmt.bind_int(1, session.subject_id);
  if (session.task_id) stmt.bind_int(2, *session.task_id);
  else stmt.bind_null(2);
  stmt.bind_text(3, session.start_time);
  if (session.end_time) stmt.bind_text(4, *session.end_time);
  else stmt.bind_null(4);
  stmt.bind_int(5, session.duration_seconds);
  stmt.bind_text(6, session.notes);
  stmt.bind_int(7, session.id);
  stmt.step();
}

void SessionRepo::remove(int64_t id) {
  db::Statement stmt(conn_.handle(), "DELETE FROM study_sessions WHERE id = ?");
  stmt.bind_int(1, id);
  stmt.step();
}

int64_t SessionRepo::total_duration_by_subject(int64_t subject_id, const std::string& start, const std::string& end) {
  auto sql = fmt::format(
    "SELECT COALESCE(SUM(duration_seconds), 0) FROM study_sessions "
    "WHERE subject_id = {} AND start_time >= '{}' AND start_time < '{}' AND end_time IS NOT NULL",
    subject_id, start, end);
  db::Statement stmt(conn_.handle(), sql);
  stmt.step();
  return stmt.column_int64(0);
}

int64_t SessionRepo::total_duration_by_date(const std::string& date) {
  auto sql = fmt::format(
    "SELECT COALESCE(SUM(duration_seconds), 0) FROM study_sessions "
    "WHERE date(start_time) = '{}' AND end_time IS NOT NULL", date);
  db::Statement stmt(conn_.handle(), sql);
  stmt.step();
  return stmt.column_int64(0);
}

} // namespace study_planner
