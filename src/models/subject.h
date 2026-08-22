#pragma once
#include <string>
#include <nlohmann/json.hpp>

namespace study_planner {

struct Subject {
  int64_t id = 0;
  std::string name;
  std::string description;
  std::string color = "#4A90D9";
  std::string created_at;

  nlohmann::json to_json() const {
    return {
      {"id", id},
      {"name", name},
      {"description", description},
      {"color", color},
      {"created_at", created_at}
    };
  }

  static Subject from_json(const nlohmann::json& j) {
    Subject s;
    s.id = j.value("id", 0);
    s.name = j.value("name", "");
    s.description = j.value("description", "");
    s.color = j.value("color", "#4A90D9");
    s.created_at = j.value("created_at", "");
    return s;
  }
};

} // namespace study_planner
