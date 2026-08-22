#pragma once
#include <sqlite3.h>
#include <string>
#include <string_view>
#include <stdexcept>

namespace study_planner::db {

class Statement {
public:
  Statement() = default;
  explicit Statement(sqlite3* db, std::string_view sql);
  ~Statement();

  Statement(const Statement&) = delete;
  Statement& operator=(const Statement&) = delete;
  Statement(Statement&& other) noexcept;
  Statement& operator=(Statement&& other) noexcept;

  void prepare(sqlite3* db, std::string_view sql);
  void reset();
  bool step();

  void bind_int(int index, int64_t value);
  void bind_double(int index, double value);
  void bind_text(int index, std::string_view value);
  void bind_null(int index);

  int64_t column_int64(int index) const;
  double column_double(int index) const;
  std::string column_text(int index) const;
  bool column_is_null(int index) const;

  sqlite3_stmt* handle() const { return stmt_; }

private:
  sqlite3_stmt* stmt_ = nullptr;
};

} // namespace study_planner::db
