#pragma once
#include "db/connection.h"

namespace study_planner::db {

class MigrationRunner {
public:
  explicit MigrationRunner(Connection& conn) : conn_(conn) {}

  void run();

private:
  Connection& conn_;

  void migrate_v1();
};

} // namespace study_planner::db
