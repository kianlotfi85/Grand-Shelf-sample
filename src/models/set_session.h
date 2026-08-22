#pragma once
#include <string>
#include <optional>
#include <nlohmann/json.hpp>

namespace study_planner {

struct SetSession {
  int64_t id = 0;
  int64_t set_id = 0;
  std::optional<int64_t> subject_id;
  std::string session_type; // work, break, long_break
  std::string start_time;
  std::optional<std::string> end_time;
  std::string created_at;

  nlohmann::json to_json() const {
    nlohmann::json j;
    j["id"] = id;
    j["set_id"] = set_id;
    j["session_type"] = session_type;
    j["start_time"] = start_time;
    j["created_at"] = created_at;
    if (subject_id) j["subject_id"] = *subject_id;
    if (end_time) j["end_time"] = *end_time;
    return j;
  }

  static SetSession from_json(const nlohmann::json& j) {
    SetSession s;
    s.id = j.value("id", 0);
    s.set_id = j.value("set_id", 0);
    s.session_type = j.value("session_type", "work");
    s.start_time = j.value("start_time", "");
    s.created_at = j.value("created_at", "");
    if (j.contains("subject_id") && !j["subject_id"].is_null())
      s.subject_id = j["subject_id"].get<int64_t>();
    if (j.contains("end_time") && !j["end_time"].is_null())
      s.end_time = j["end_time"].get<std::string>();
    return s;
  }
};

} // namespace study_planner
