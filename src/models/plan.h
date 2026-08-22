#pragma once
#include <string>
#include <nlohmann/json.hpp>

namespace study_planner {

struct Plan {
  int64_t id = 0;
  int64_t subject_id = 0;
  std::string plan_type; // daily, weekly, monthly
  std::string target_date;
  int64_t target_minutes = 0;
  std::string description;
  bool is_completed = false;
  std::string created_at;

  nlohmann::json to_json() const {
    return {
      {"id", id},
      {"subject_id", subject_id},
      {"plan_type", plan_type},
      {"target_date", target_date},
      {"target_minutes", target_minutes},
      {"description", description},
      {"is_completed", is_completed},
      {"created_at", created_at}
    };
  }

  static Plan from_json(const nlohmann::json& j) {
    Plan p;
    p.id = j.value("id", 0);
    p.subject_id = j.value("subject_id", 0);
    p.plan_type = j.value("plan_type", "daily");
    p.target_date = j.value("target_date", "");
    p.target_minutes = j.value("target_minutes", 0);
    p.description = j.value("description", "");
    p.is_completed = j.value("is_completed", false);
    p.created_at = j.value("created_at", "");
    return p;
  }
};

} // namespace study_planner
