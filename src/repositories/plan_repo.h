#pragma once
#include "db/connection.h"
#include "models/plan.h"
#include "models/plan_block.h"
#include <vector>
#include <optional>

namespace study_planner {

class PlanRepo {
public:
  explicit PlanRepo(db::Connection& conn) : conn_(conn) {}

  int64_t create(const Plan& plan);
  std::optional<Plan> find_by_id(int64_t id);
  std::vector<Plan> find_by_subject(int64_t subject_id);
  std::vector<Plan> find_by_date(const std::string& target_date);
  std::vector<Plan> find_all();
  void update(const Plan& plan);
  void remove(int64_t id);

  // Plan blocks
  int64_t create_block(const PlanBlock& block);
  std::vector<PlanBlock> find_blocks_by_plan(int64_t plan_id);
  void update_block(const PlanBlock& block);
  void remove_block(int64_t block_id);

private:
  db::Connection& conn_;
};

} // namespace study_planner
