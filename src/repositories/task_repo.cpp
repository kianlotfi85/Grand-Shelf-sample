#include "repositories/task_repo.h"
#include "db/statement.h"
#include <fmt/core.h>

namespace study_planner {

static Task parse_task_row(db::Statement& stmt) {
  Task t;
  t.id = stmt.column_int64(0);
  t.subject_id = stmt.column_int64(1);
  t.title = stmt.column_text(2);
  t.description = stmt.column_text(3);
  t.priority = static_cast<int>(stmt.column_int64(4));
  t.status = stmt.column_text(5);
  if (!stmt.column_is_null(6)) t.due_date = stmt.column_text(6);
  t.estimated_minutes = stmt.column_int64(7);
  t.created_at = stmt.column_text(8);
  if (!stmt.column_is_null(9)) t.completed_at = stmt.column_text(9);
  return t;
}

int64_t TaskRepo::create(const Task& task) {
  db::Statement stmt(conn_.handle(),
    "INSERT INTO tasks (subject_id, title, description, priority, status, due_date, estimated_minutes) "
    "VALUES (?, ?, ?, ?, ?, ?, ?)");
  stmt.bind_int(1, task.subject_id);
  stmt.bind_text(2, task.title);
  stmt.bind_text(3, task.description);
  stmt.bind_int(4, task.priority);
  stmt.bind_text(5, task.status);
  if (task.due_date) stmt.bind_text(6, *task.due_date);
  else stmt.bind_null(6);
  stmt.bind_int(7, task.estimated_minutes);
  stmt.step();
  return conn_.last_insert_rowid();
}

std::optional<Task> TaskRepo::find_by_id(int64_t id) {
  db::Statement stmt(conn_.handle(),
    "SELECT id, subject_id, title, description, priority, status, due_date, "
    "estimated_minutes, created_at, completed_at FROM tasks WHERE id = ?");
  stmt.bind_int(1, id);
  if (!stmt.step()) return std::nullopt;
  return parse_task_row(stmt);
}

std::vector<Task> TaskRepo::find_by_subject(int64_t subject_id) {
  std::vector<Task> result;
  db::Statement stmt(conn_.handle(),
    "SELECT id, subject_id, title, description, priority, status, due_date, "
    "estimated_minutes, created_at, completed_at FROM tasks WHERE subject_id = ? ORDER BY priority DESC, created_at DESC");
  stmt.bind_int(1, subject_id);
  while (stmt.step()) {
    result.push_back(parse_task_row(stmt));
  }
  return result;
}

std::vector<Task> TaskRepo::find_by_filter(const TaskFilter& filter) {
  std::string sql = "SELECT id, subject_id, title, description, priority, status, due_date, "
                    "estimated_minutes, created_at, completed_at FROM tasks WHERE 1=1";
  bool has_where = true;

  if (filter.status) {
    sql += " AND status = '" + *filter.status + "'";
  }
  if (filter.subject_id) {
    sql += fmt::format(" AND subject_id = {}", *filter.subject_id);
  }
  if (filter.priority) {
    sql += fmt::format(" AND priority = {}", *filter.priority);
  }

  std::string order = filter.sort_desc ? " DESC" : " ASC";
  sql += " ORDER BY " + filter.sort_by + order;

  std::vector<Task> result;
  db::Statement stmt(conn_.handle(), sql);
  while (stmt.step()) {
    result.push_back(parse_task_row(stmt));
  }
  return result;
}

std::vector<Task> TaskRepo::find_all() {
  return find_by_filter(TaskFilter{});
}

void TaskRepo::update(const Task& task) {
  db::Statement stmt(conn_.handle(),
    "UPDATE tasks SET subject_id = ?, title = ?, description = ?, priority = ?, "
    "status = ?, due_date = ?, estimated_minutes = ?, completed_at = ? WHERE id = ?");
  stmt.bind_int(1, task.subject_id);
  stmt.bind_text(2, task.title);
  stmt.bind_text(3, task.description);
  stmt.bind_int(4, task.priority);
  stmt.bind_text(5, task.status);
  if (task.due_date) stmt.bind_text(6, *task.due_date);
  else stmt.bind_null(6);
  stmt.bind_int(7, task.estimated_minutes);
  if (task.completed_at) stmt.bind_text(8, *task.completed_at);
  else stmt.bind_null(8);
  stmt.bind_int(9, task.id);
  stmt.step();
}

void TaskRepo::remove(int64_t id) {
  db::Statement stmt(conn_.handle(), "DELETE FROM tasks WHERE id = ?");
  stmt.bind_int(1, id);
  stmt.step();
}

} // namespace study_planner
