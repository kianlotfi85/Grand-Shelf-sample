#include "db/migrations.h"
#include <fmt/core.h>
#include "db/statement.h"

namespace study_planner::db {

void MigrationRunner::run() {
  conn_.exec(R"(
    CREATE TABLE IF NOT EXISTS schema_version (
      version INTEGER PRIMARY KEY
    );
  )");

  int current = 0;
  {
    auto stmt = Statement(conn_.handle(), "SELECT COALESCE(MAX(version), 0) FROM schema_version");
    if (stmt.step()) {
      current = static_cast<int>(stmt.column_int64(0));
    }
  }

  if (current < 1) {
    fmt::print("  Running migration v1...\n");
    migrate_v1();
    conn_.exec("INSERT OR REPLACE INTO schema_version (version) VALUES (1)");
  }
}

void MigrationRunner::migrate_v1() {
  conn_.exec(R"(
    CREATE TABLE IF NOT EXISTS subjects (
      id          INTEGER PRIMARY KEY AUTOINCREMENT,
      name        TEXT NOT NULL UNIQUE,
      description TEXT DEFAULT '',
      color       TEXT DEFAULT '#4A90D9',
      created_at  TEXT NOT NULL DEFAULT (datetime('now'))
    );

    CREATE TABLE IF NOT EXISTS tasks (
      id                INTEGER PRIMARY KEY AUTOINCREMENT,
      subject_id        INTEGER NOT NULL REFERENCES subjects(id) ON DELETE CASCADE,
      title             TEXT NOT NULL,
      description       TEXT DEFAULT '',
      priority          INTEGER NOT NULL DEFAULT 0,
      status            TEXT NOT NULL DEFAULT 'todo',
      due_date          TEXT,
      estimated_minutes INTEGER DEFAULT 0,
      created_at        TEXT NOT NULL DEFAULT (datetime('now')),
      completed_at      TEXT
    );

    CREATE TABLE IF NOT EXISTS study_sessions (
      id               INTEGER PRIMARY KEY AUTOINCREMENT,
      subject_id       INTEGER NOT NULL REFERENCES subjects(id) ON DELETE CASCADE,
      task_id          INTEGER REFERENCES tasks(id) ON DELETE SET NULL,
      start_time       TEXT NOT NULL,
      end_time         TEXT,
      duration_seconds INTEGER DEFAULT 0,
      notes            TEXT DEFAULT '',
      created_at       TEXT NOT NULL DEFAULT (datetime('now'))
    );

    CREATE TABLE IF NOT EXISTS study_sets (
      id                         INTEGER PRIMARY KEY AUTOINCREMENT,
      name                       TEXT NOT NULL,
      description                TEXT DEFAULT '',
      work_duration_seconds      INTEGER NOT NULL DEFAULT 1500,
      break_duration_seconds     INTEGER NOT NULL DEFAULT 300,
      long_break_seconds         INTEGER NOT NULL DEFAULT 900,
      sessions_before_long_break INTEGER NOT NULL DEFAULT 4,
      created_at                 TEXT NOT NULL DEFAULT (datetime('now'))
    );

    CREATE TABLE IF NOT EXISTS set_sessions (
      id           INTEGER PRIMARY KEY AUTOINCREMENT,
      set_id       INTEGER NOT NULL REFERENCES study_sets(id) ON DELETE CASCADE,
      subject_id   INTEGER REFERENCES subjects(id) ON DELETE SET NULL,
      session_type TEXT NOT NULL,
      start_time   TEXT NOT NULL,
      end_time     TEXT,
      created_at   TEXT NOT NULL DEFAULT (datetime('now'))
    );

    CREATE TABLE IF NOT EXISTS plans (
      id             INTEGER PRIMARY KEY AUTOINCREMENT,
      subject_id     INTEGER NOT NULL REFERENCES subjects(id) ON DELETE CASCADE,
      plan_type      TEXT NOT NULL,
      target_date    TEXT NOT NULL,
      target_minutes INTEGER NOT NULL,
      description    TEXT DEFAULT '',
      is_completed   INTEGER NOT NULL DEFAULT 0,
      created_at     TEXT NOT NULL DEFAULT (datetime('now'))
    );

    CREATE TABLE IF NOT EXISTS plan_blocks (
      id           INTEGER PRIMARY KEY AUTOINCREMENT,
      plan_id      INTEGER NOT NULL REFERENCES plans(id) ON DELETE CASCADE,
      subject_id   INTEGER NOT NULL REFERENCES subjects(id) ON DELETE CASCADE,
      start_time   TEXT NOT NULL,
      end_time     TEXT NOT NULL,
      day_of_week  INTEGER DEFAULT -1,
      created_at   TEXT NOT NULL DEFAULT (datetime('now'))
    );

    CREATE INDEX IF NOT EXISTS idx_sessions_subject_time ON study_sessions(subject_id, start_time);
    CREATE INDEX IF NOT EXISTS idx_sessions_task ON study_sessions(task_id);
    CREATE INDEX IF NOT EXISTS idx_tasks_subject_status ON tasks(subject_id, status);
    CREATE INDEX IF NOT EXISTS idx_plans_subject_date ON plans(subject_id, target_date, plan_type);
  )");
}

} // namespace study_planner::db
