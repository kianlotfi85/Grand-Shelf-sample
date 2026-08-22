#include "db/connection.h"
#include <fmt/core.h>

namespace study_planner::db {

Connection::~Connection() {
  close();
}

void Connection::open(const std::string& path) {
  if (db_) close();
  int rc = sqlite3_open(path.c_str(), &db_);
  if (rc != SQLITE_OK) {
    std::string err = db_ ? sqlite3_errmsg(db_) : "unknown error";
    throw DatabaseError(fmt::format("Cannot open database '{}': {}", path, err));
  }
  sqlite3_exec(db_, "PRAGMA journal_mode=WAL;", nullptr, nullptr, nullptr);
  sqlite3_exec(db_, "PRAGMA foreign_keys=ON;", nullptr, nullptr, nullptr);
}

void Connection::close() {
  if (db_) {
    sqlite3_close(db_);
    db_ = nullptr;
  }
}

int64_t Connection::last_insert_rowid() const {
  return sqlite3_last_insert_rowid(db_);
}

void Connection::exec(const std::string& sql) {
  char* errmsg = nullptr;
  int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errmsg);
  if (rc != SQLITE_OK) {
    std::string err(errmsg ? errmsg : "unknown error");
    sqlite3_free(errmsg);
    throw DatabaseError(fmt::format("SQL error: {}", err));
  }
}

bool Connection::table_exists(const std::string& name) {
  auto sql = fmt::format(
    "SELECT count(*) FROM sqlite_master WHERE type='table' AND name='{}'", name);
  sqlite3_stmt* stmt = nullptr;
  sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
  sqlite3_step(stmt);
  bool exists = sqlite3_column_int(stmt, 0) > 0;
  sqlite3_finalize(stmt);
  return exists;
}

} // namespace study_planner::db
