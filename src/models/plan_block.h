#pragma once
#include <string>
#include <nlohmann/json.hpp>

namespace study_planner {

struct PlanBlock {
  int64_t id = 0;
  int64_t plan_id = 0;
  int64_t subject_id = 0;
  std::string start_time; // HH:MM
  std::string end_time;   // HH:MM
  int day_of_week = -1;   // -1 = all days, 0=Sun..6=Sat
  std::string created_at;

  nlohmann::json to_json() const {
    return {
      {"id", id},
      {"plan_id", plan_id},
      {"subject_id", subject_id},
      {"start_time", start_time},
      {"end_time", end_time},
      {"day_of_week", day_of_week},
      {"created_at", created_at}
    };
  }

  static PlanBlock from_json(const nlohmann::json& j) {
    PlanBlock b;
    b.id = j.value("id", 0);
    b.plan_id = j.value("plan_id", 0);
    b.subject_id = j.value("subject_id", 0);
    b.start_time = j.value("start_time", "");
    b.end_time = j.value("end_time", "");
    b.day_of_week = j.value("day_of_week", -1);
    b.created_at = j.value("created_at", "");
    return b;
  }
};

} // namespace study_planner
