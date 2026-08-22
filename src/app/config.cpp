#include "app/config.h"
#include <fstream>
#include <fmt/core.h>

namespace study_planner {

Config Config::load(const std::string& path) {
  Config config;
  std::ifstream ifs(path);
  if (!ifs.is_open()) {
    fmt::print(stderr, "Warning: could not open config '{}', using defaults\n", path);
    return config;
  }
  try {
    auto j = nlohmann::json::parse(ifs);
    if (j.contains("database")) {
      config.database.path = j["database"].value("path", config.database.path);
    }
    if (j.contains("defaults") && j["defaults"].contains("pomodoro")) {
      auto& p = j["defaults"]["pomodoro"];
      config.defaults.work_duration_seconds = p.value("work_duration_seconds", config.defaults.work_duration_seconds);
      config.defaults.break_duration_seconds = p.value("break_duration_seconds", config.defaults.break_duration_seconds);
      config.defaults.long_break_seconds = p.value("long_break_seconds", config.defaults.long_break_seconds);
      config.defaults.sessions_before_long_break = p.value("sessions_before_long_break", config.defaults.sessions_before_long_break);
    }
    if (j.contains("ui")) {
      config.ui.table_width = j["ui"].value("table_width", config.ui.table_width);
      config.ui.date_format = j["ui"].value("date_format", config.ui.date_format);
      config.ui.datetime_format = j["ui"].value("datetime_format", config.ui.datetime_format);
    }
    if (j.contains("ai")) {
      config.ai.enabled = j["ai"].value("enabled", config.ai.enabled);
      config.ai.provider = j["ai"].value("provider", config.ai.provider);
      config.ai.api_key_path = j["ai"].value("api_key_path", config.ai.api_key_path);
      config.ai.model = j["ai"].value("model", config.ai.model);
      config.ai.temperature = j["ai"].value("temperature", config.ai.temperature);
    }
  } catch (const std::exception& e) {
    fmt::print(stderr, "Warning: failed to parse config '{}': {}\n", path, e.what());
  }
  return config;
}

void Config::save(const std::string& path) const {
  nlohmann::json j;
  j["database"]["path"] = database.path;
  j["defaults"]["pomodoro"]["work_duration_seconds"] = defaults.work_duration_seconds;
  j["defaults"]["pomodoro"]["break_duration_seconds"] = defaults.break_duration_seconds;
  j["defaults"]["pomodoro"]["long_break_seconds"] = defaults.long_break_seconds;
  j["defaults"]["pomodoro"]["sessions_before_long_break"] = defaults.sessions_before_long_break;
  j["ui"]["table_width"] = ui.table_width;
  j["ui"]["date_format"] = ui.date_format;
  j["ui"]["datetime_format"] = ui.datetime_format;
  j["ai"]["enabled"] = ai.enabled;
  j["ai"]["provider"] = ai.provider;
  j["ai"]["api_key_path"] = ai.api_key_path;
  j["ai"]["model"] = ai.model;
  j["ai"]["temperature"] = ai.temperature;
  std::ofstream ofs(path);
  ofs << j.dump(2) << "\n";
}

} // namespace study_planner
