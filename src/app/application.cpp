#include "app/application.h"
#include "db/migrations.h"
#include <iostream>
#include <fmt/core.h>
#include <filesystem>

namespace study_planner {

int Application::run(int argc, char* argv[]) {
  parse_args(argc, argv);

  // Resolve paths
  namespace fs = std::filesystem;
  if (config_path_.empty()) {
    // Try config in same dir as executable, then relative paths
    if (fs::exists("config/default_config.json"))
      config_path_ = "config/default_config.json";
    else if (fs::exists("../config/default_config.json"))
      config_path_ = "../config/default_config.json";
  }

  config_ = Config::load(config_path_);

  if (db_path_.empty()) {
    db_path_ = config_.database.path;
  }

  init_database();

  // Launch UI
  ui::Console console(db_, config_);
  console.run();

  return 0;
}

void Application::parse_args(int argc, char* argv[]) {
  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];
    if (arg == "--db" && i + 1 < argc) {
      db_path_ = argv[++i];
    } else if (arg == "--config" && i + 1 < argc) {
      config_path_ = argv[++i];
    } else if (arg == "--help" || arg == "-h") {
      fmt::print("Study Planner v1.0\n\n");
      fmt::print("Usage: study-planner [options]\n\n");
      fmt::print("Options:\n");
      fmt::print("  --db PATH      Database file path (default: study_planner.db)\n");
      fmt::print("  --config PATH  Config file path\n");
      fmt::print("  --help, -h     Show this help\n");
      std::exit(0);
    }
  }
}

void Application::init_database() {
  fmt::print("Initializing database: {}\n", db_path_);
  db_.open(db_path_);

  db::MigrationRunner migrations(db_);
  migrations.run();

  fmt::print("Database ready.\n");
}

} // namespace study_planner
