#pragma once
#include <sqlite3.h>
#include <string>
#include <memory>
#include <stdexcept>

namespace study_planner::db {

class DatabaseError : public std::runtime_error {
public:
  explicit DatabaseError(const std::string& msg) : std::runtime_error(msg) {}
};

class Connection {
public:
  Connection() = default;
  ~Connection();

  Connection(const Connection&) = delete;
  Connection& operator=(const Connection&) = delete;
  Connection(Connection&&) = default;
  Connection& operator=(Connection&&) = default;

  void open(const std::string& path);
  void close();
  bool is_open() const { return db_ != nullptr; }

  sqlite3* handle() { return db_; }
  int64_t last_insert_rowid() const;

  void exec(const std::string& sql);
  bool table_exists(const std::string& name);

private:
  sqlite3* db_ = nullptr;
};

} // namespace study_planner::db
