#pragma once
#include <string>
#include <nlohmann/json.hpp>

namespace study_planner {

struct Config {
  struct Database {
    std::string path = "study_planner.db";
  };

  struct PomodoroDefaults {
    int64_t work_duration_seconds = 1500;
    int64_t break_duration_seconds = 300;
    int64_t long_break_seconds = 900;
    int64_t sessions_before_long_break = 4;
  };

  struct UI {
    int table_width = 80;
    std::string date_format = "%Y-%m-%d";
    std::string datetime_format = "%Y-%m-%d %H:%M:%S";
  };

  struct AI {
    bool enabled = false;
    std::string provider = "openai";
    std::string api_key_path;
    std::string model = "gpt-4";
    double temperature = 0.7;
  };

  Database database;
  PomodoroDefaults defaults;
  UI ui;
  AI ai;

  static Config load(const std::string& path);
  void save(const std::string& path) const;
};

} // namespace study_planner
