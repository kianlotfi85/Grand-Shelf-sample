#pragma once
#include <string>
#include <nlohmann/json.hpp>

namespace study_planner {

struct StudySet {
  int64_t id = 0;
  std::string name;
  std::string description;
  int64_t work_duration_seconds = 1500;
  int64_t break_duration_seconds = 300;
  int64_t long_break_seconds = 900;
  int64_t sessions_before_long_break = 4;
  std::string created_at;

  nlohmann::json to_json() const {
    return {
      {"id", id},
      {"name", name},
      {"description", description},
      {"work_duration_seconds", work_duration_seconds},
      {"break_duration_seconds", break_duration_seconds},
      {"long_break_seconds", long_break_seconds},
      {"sessions_before_long_break", sessions_before_long_break},
      {"created_at", created_at}
    };
  }

  static StudySet from_json(const nlohmann::json& j) {
    StudySet s;
    s.id = j.value("id", 0);
    s.name = j.value("name", "");
    s.description = j.value("description", "");
    s.work_duration_seconds = j.value("work_duration_seconds", 1500);
    s.break_duration_seconds = j.value("break_duration_seconds", 300);
    s.long_break_seconds = j.value("long_break_seconds", 900);
    s.sessions_before_long_break = j.value("sessions_before_long_break", 4);
    s.created_at = j.value("created_at", "");
    return s;
  }
};

} // namespace study_planner
