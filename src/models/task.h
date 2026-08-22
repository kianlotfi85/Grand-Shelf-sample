#pragma once
#include <string>
#include <optional>
#include <nlohmann/json.hpp>

namespace study_planner {

struct Task {
  int64_t id = 0;
  int64_t subject_id = 0;
  std::string title;
  std::string description;
  int priority = 0; // 0=low, 1=med, 2=high
  std::string status = "todo"; // todo, in_progress, completed
  std::optional<std::string> due_date;
  int estimated_minutes = 0;
  std::string created_at;
  std::optional<std::string> completed_at;

  nlohmann::json to_json() const {
    nlohmann::json j;
    j["id"] = id;
    j["subject_id"] = subject_id;
    j["title"] = title;
    j["description"] = description;
    j["priority"] = priority;
    j["status"] = status;
    j["estimated_minutes"] = estimated_minutes;
    j["created_at"] = created_at;
    if (due_date) j["due_date"] = *due_date;
    if (completed_at) j["completed_at"] = *completed_at;
    return j;
  }

  static Task from_json(const nlohmann::json& j) {
    Task t;
    t.id = j.value("id", 0);
    t.subject_id = j.value("subject_id", 0);
    t.title = j.value("title", "");
    t.description = j.value("description", "");
    t.priority = j.value("priority", 0);
    t.status = j.value("status", "todo");
    t.estimated_minutes = j.value("estimated_minutes", 0);
    t.created_at = j.value("created_at", "");
    if (j.contains("due_date") && !j["due_date"].is_null())
      t.due_date = j["due_date"].get<std::string>();
    if (j.contains("completed_at") && !j["completed_at"].is_null())
      t.completed_at = j["completed_at"].get<std::string>();
    return t;
  }
};

} // namespace study_planner
