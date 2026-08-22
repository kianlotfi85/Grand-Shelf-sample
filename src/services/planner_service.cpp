#include "services/planner_service.h"
#include <fmt/core.h>

namespace study_planner {

int64_t PlannerService::create_plan(int64_t subject_id, const std::string& plan_type,
                                     const std::string& target_date, int64_t target_minutes,
                                     const std::string& description) {
  Plan plan;
  plan.subject_id = subject_id;
  plan.plan_type = plan_type;
  plan.target_date = target_date;
  plan.target_minutes = target_minutes;
  plan.description = description;
  plan.is_completed = false;
  return plan_repo_.create(plan);
}

std::optional<Plan> PlannerService::get_plan(int64_t id) {
  return plan_repo_.find_by_id(id);
}

std::vector<Plan> PlannerService::get_plans_by_subject(int64_t subject_id) {
  return plan_repo_.find_by_subject(subject_id);
}

std::vector<Plan> PlannerService::get_plans_by_date(const std::string& date) {
  return plan_repo_.find_by_date(date);
}

std::vector<Plan> PlannerService::get_all_plans() {
  return plan_repo_.find_all();
}

void PlannerService::update_plan(const Plan& plan) {
  plan_repo_.update(plan);
}

void PlannerService::delete_plan(int64_t id) {
  plan_repo_.remove(id);
}

int64_t PlannerService::add_block(int64_t plan_id, int64_t subject_id,
                                   const std::string& start_time, const std::string& end_time,
                                   int day_of_week) {
  PlanBlock block;
  block.plan_id = plan_id;
  block.subject_id = subject_id;
  block.start_time = start_time;
  block.end_time = end_time;
  block.day_of_week = day_of_week;
  return plan_repo_.create_block(block);
}

std::vector<PlanBlock> PlannerService::get_blocks(int64_t plan_id) {
  return plan_repo_.find_blocks_by_plan(plan_id);
}

void PlannerService::remove_block(int64_t block_id) {
  plan_repo_.remove_block(block_id);
}

std::vector<PlanProgress> PlannerService::get_progress(const std::string& date) {
  std::vector<PlanProgress> result;
  auto plans = plan_repo_.find_by_date(date);
  for (auto& plan : plans) {
    PlanProgress pp;
    pp.plan = plan;
    auto subj = subject_repo_.find_by_id(plan.subject_id);
    pp.subject_name = subj ? subj->name : "Unknown";
    auto day_end = date + " 23:59:59";
    pp.actual_minutes = session_repo_.total_duration_by_subject(plan.subject_id, date, day_end) / 60;
    pp.completion_pct = plan.target_minutes > 0
      ? (static_cast<double>(pp.actual_minutes) / plan.target_minutes) * 100.0
      : 0.0;
    result.push_back(pp);
  }
  return result;
}

} // namespace study_planner
