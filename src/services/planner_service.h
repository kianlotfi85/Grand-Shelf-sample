#pragma once
#include "repositories/plan_repo.h"
#include "repositories/session_repo.h"
#include "repositories/subject_repo.h"
#include "models/plan.h"
#include "models/plan_block.h"
#include <vector>
#include <optional>
#include <string>

namespace study_planner {

struct PlanProgress {
  Plan plan;
  std::string subject_name;
  int64_t actual_minutes;
  double completion_pct;
};

class PlannerService {
public:
  PlannerService(PlanRepo& plan_repo, SessionRepo& session_repo, SubjectRepo& subject_repo)
    : plan_repo_(plan_repo), session_repo_(session_repo), subject_repo_(subject_repo) {}

  int64_t create_plan(int64_t subject_id, const std::string& plan_type,
                      const std::string& target_date, int64_t target_minutes,
                      const std::string& description);
  std::optional<Plan> get_plan(int64_t id);
  std::vector<Plan> get_plans_by_subject(int64_t subject_id);
  std::vector<Plan> get_plans_by_date(const std::string& date);
  std::vector<Plan> get_all_plans();
  void update_plan(const Plan& plan);
  void delete_plan(int64_t id);

  int64_t add_block(int64_t plan_id, int64_t subject_id,
                    const std::string& start_time, const std::string& end_time,
                    int day_of_week);
  std::vector<PlanBlock> get_blocks(int64_t plan_id);
  void remove_block(int64_t block_id);

  std::vector<PlanProgress> get_progress(const std::string& date);

private:
  PlanRepo& plan_repo_;
  SessionRepo& session_repo_;
  SubjectRepo& subject_repo_;
};

} // namespace study_planner
