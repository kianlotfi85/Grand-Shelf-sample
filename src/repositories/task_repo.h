#pragma once
#include "db/connection.h"
#include "models/task.h"
#include <vector>
#include <optional>
#include <string>

namespace study_planner {

struct TaskFilter {
  std::optional<std::string> status;
  std::optional<int64_t> subject_id;
  std::optional<int> priority;
  std::string sort_by = "created_at";
  bool sort_desc = false;
};

class TaskRepo {
public:
  explicit TaskRepo(db::Connection& conn) : conn_(conn) {}

  int64_t create(const Task& task);
  std::optional<Task> find_by_id(int64_t id);
  std::vector<Task> find_by_subject(int64_t subject_id);
  std::vector<Task> find_by_filter(const TaskFilter& filter);
  std::vector<Task> find_all();
  void update(const Task& task);
  void remove(int64_t id);

private:
  db::Connection& conn_;
};

} // namespace study_planner
