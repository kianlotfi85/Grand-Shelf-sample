#pragma once
#include "db/connection.h"
#include "app/config.h"
#include "ui/console.h"
#include <string>

namespace study_planner {

class Application {
public:
  Application() = default;
  int run(int argc, char* argv[]);

private:
  void parse_args(int argc, char* argv[]);
  void init_database();
  std::string resolve_config_path();

  Config config_;
  db::Connection db_;
  std::string db_path_;
  std::string config_path_;
};

} // namespace study_planner
