#pragma once
#include <string>
#include <optional>
#include <nlohmann/json.hpp>

namespace study_planner {

struct StudySession {
  int64_t id = 0;
  int64_t subject_id = 0;
  std::optional<int64_t> task_id;
  std::string start_time;
  std::optional<std::string> end_time;
  int64_t duration_seconds = 0;
  std::string notes;
  std::string created_at;

  nlohmann::json to_json() const {
    nlohmann::json j;
    j["id"] = id;
    j["subject_id"] = subject_id;
    j["start_time"] = start_time;
    j["duration_seconds"] = duration_seconds;
    j["notes"] = notes;
    j["created_at"] = created_at;
    if (task_id) j["task_id"] = *task_id;
    if (end_time) j["end_time"] = *end_time;
    return j;
  }

  static StudySession from_json(const nlohmann::json& j) {
    StudySession s;
    s.id = j.value("id", 0);
    s.subject_id = j.value("subject_id", 0);
    s.start_time = j.value("start_time", "");
    s.duration_seconds = j.value("duration_seconds", 0);
    s.notes = j.value("notes", "");
    s.created_at = j.value("created_at", "");
    if (j.contains("task_id") && !j["task_id"].is_null())
      s.task_id = j["task_id"].get<int64_t>();
    if (j.contains("end_time") && !j["end_time"].is_null())
      s.end_time = j["end_time"].get<std::string>();
    return s;
  }
};

} // namespace study_planner
