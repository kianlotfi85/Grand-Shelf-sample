#include "db/statement.h"
#include <fmt/core.h>

namespace study_planner::db {

Statement::Statement(sqlite3* db, std::string_view sql) {
  prepare(db, sql);
}

Statement::~Statement() {
  if (stmt_) sqlite3_finalize(stmt_);
}

Statement::Statement(Statement&& other) noexcept : stmt_(other.stmt_) {
  other.stmt_ = nullptr;
}

Statement& Statement::operator=(Statement&& other) noexcept {
  if (this != &other) {
    if (stmt_) sqlite3_finalize(stmt_);
    stmt_ = other.stmt_;
    other.stmt_ = nullptr;
  }
  return *this;
}

void Statement::prepare(sqlite3* db, std::string_view sql) {
  if (stmt_) {
    sqlite3_finalize(stmt_);
    stmt_ = nullptr;
  }
  int rc = sqlite3_prepare_v2(db, sql.data(), -1, &stmt_, nullptr);
  if (rc != SQLITE_OK) {
    throw std::runtime_error(fmt::format("Prepare error: {}", sqlite3_errmsg(db)));
  }
}

void Statement::reset() {
  sqlite3_reset(stmt_);
  sqlite3_clear_bindings(stmt_);
}

bool Statement::step() {
  int rc = sqlite3_step(stmt_);
  if (rc == SQLITE_ROW) return true;
  if (rc == SQLITE_DONE) return false;
  throw std::runtime_error(fmt::format("Step error: {}", sqlite3_errmsg(sqlite3_db_handle(stmt_))));
}

void Statement::bind_int(int index, int64_t value) {
  sqlite3_bind_int64(stmt_, index, value);
}

void Statement::bind_double(int index, double value) {
  sqlite3_bind_double(stmt_, index, value);
}

void Statement::bind_text(int index, std::string_view value) {
  sqlite3_bind_text(stmt_, index, value.data(), value.size(), SQLITE_TRANSIENT);
}

void Statement::bind_null(int index) {
  sqlite3_bind_null(stmt_, index);
}

int64_t Statement::column_int64(int index) const {
  return sqlite3_column_int64(stmt_, index);
}

double Statement::column_double(int index) const {
  return sqlite3_column_double(stmt_, index);
}

std::string Statement::column_text(int index) const {
  auto* text = reinterpret_cast<const char*>(sqlite3_column_text(stmt_, index));
  return text ? std::string(text) : std::string{};
}

bool Statement::column_is_null(int index) const {
  return sqlite3_column_type(stmt_, index) == SQLITE_NULL;
}

} // namespace study_planner::db
