#include "repositories/subject_repo.h"
#include "db/statement.h"

namespace study_planner {

int64_t SubjectRepo::create(const Subject& subject) {
  db::Statement stmt(conn_.handle(),
    "INSERT INTO subjects (name, description, color) VALUES (?, ?, ?)");
  stmt.bind_text(1, subject.name);
  stmt.bind_text(2, subject.description);
  stmt.bind_text(3, subject.color);
  stmt.step();
  return conn_.last_insert_rowid();
}

std::optional<Subject> SubjectRepo::find_by_id(int64_t id) {
  db::Statement stmt(conn_.handle(),
    "SELECT id, name, description, color, created_at FROM subjects WHERE id = ?");
  stmt.bind_int(1, id);
  if (!stmt.step()) return std::nullopt;
  Subject s;
  s.id = stmt.column_int64(0);
  s.name = stmt.column_text(1);
  s.description = stmt.column_text(2);
  s.color = stmt.column_text(3);
  s.created_at = stmt.column_text(4);
  return s;
}

std::optional<Subject> SubjectRepo::find_by_name(const std::string& name) {
  db::Statement stmt(conn_.handle(),
    "SELECT id, name, description, color, created_at FROM subjects WHERE name = ?");
  stmt.bind_text(1, name);
  if (!stmt.step()) return std::nullopt;
  Subject s;
  s.id = stmt.column_int64(0);
  s.name = stmt.column_text(1);
  s.description = stmt.column_text(2);
  s.color = stmt.column_text(3);
  s.created_at = stmt.column_text(4);
  return s;
}

std::vector<Subject> SubjectRepo::find_all() {
  std::vector<Subject> result;
  db::Statement stmt(conn_.handle(),
    "SELECT id, name, description, color, created_at FROM subjects ORDER BY name");
  while (stmt.step()) {
    Subject s;
    s.id = stmt.column_int64(0);
    s.name = stmt.column_text(1);
    s.description = stmt.column_text(2);
    s.color = stmt.column_text(3);
    s.created_at = stmt.column_text(4);
    result.push_back(std::move(s));
  }
  return result;
}

void SubjectRepo::update(const Subject& subject) {
  db::Statement stmt(conn_.handle(),
    "UPDATE subjects SET name = ?, description = ?, color = ? WHERE id = ?");
  stmt.bind_text(1, subject.name);
  stmt.bind_text(2, subject.description);
  stmt.bind_text(3, subject.color);
  stmt.bind_int(4, subject.id);
  stmt.step();
}

void SubjectRepo::remove(int64_t id) {
  db::Statement stmt(conn_.handle(), "DELETE FROM subjects WHERE id = ?");
  stmt.bind_int(1, id);
  stmt.step();
}

} // namespace study_planner
