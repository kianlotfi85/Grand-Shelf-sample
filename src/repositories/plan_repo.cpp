#include "repositories/plan_repo.h"
#include "db/statement.h"
#include <fmt/core.h>

namespace study_planner {

int64_t PlanRepo::create(const Plan& plan) {
  db::Statement stmt(conn_.handle(),
    "INSERT INTO plans (subject_id, plan_type, target_date, target_minutes, description, is_completed) "
    "VALUES (?, ?, ?, ?, ?, ?)");
  stmt.bind_int(1, plan.subject_id);
  stmt.bind_text(2, plan.plan_type);
  stmt.bind_text(3, plan.target_date);
  stmt.bind_int(4, plan.target_minutes);
  stmt.bind_text(5, plan.description);
  stmt.bind_int(6, plan.is_completed ? 1 : 0);
  stmt.step();
  return conn_.last_insert_rowid();
}

std::optional<Plan> PlanRepo::find_by_id(int64_t id) {
  db::Statement stmt(conn_.handle(),
    "SELECT id, subject_id, plan_type, target_date, target_minutes, description, is_completed, created_at "
    "FROM plans WHERE id = ?");
  stmt.bind_int(1, id);
  if (!stmt.step()) return std::nullopt;
  Plan p;
  p.id = stmt.column_int64(0);
  p.subject_id = stmt.column_int64(1);
  p.plan_type = stmt.column_text(2);
  p.target_date = stmt.column_text(3);
  p.target_minutes = stmt.column_int64(4);
  p.description = stmt.column_text(5);
  p.is_completed = stmt.column_int64(6) != 0;
  p.created_at = stmt.column_text(7);
  return p;
}

std::vector<Plan> PlanRepo::find_by_subject(int64_t subject_id) {
  std::vector<Plan> result;
  db::Statement stmt(conn_.handle(),
    "SELECT id, subject_id, plan_type, target_date, target_minutes, description, is_completed, created_at "
    "FROM plans WHERE subject_id = ? ORDER BY target_date DESC");
  stmt.bind_int(1, subject_id);
  while (stmt.step()) {
    Plan p;
    p.id = stmt.column_int64(0);
    p.subject_id = stmt.column_int64(1);
    p.plan_type = stmt.column_text(2);
    p.target_date = stmt.column_text(3);
    p.target_minutes = stmt.column_int64(4);
    p.description = stmt.column_text(5);
    p.is_completed = stmt.column_int64(6) != 0;
    p.created_at = stmt.column_text(7);
    result.push_back(std::move(p));
  }
  return result;
}

std::vector<Plan> PlanRepo::find_by_date(const std::string& target_date) {
  std::vector<Plan> result;
  auto sql = fmt::format(
    "SELECT id, subject_id, plan_type, target_date, target_minutes, description, is_completed, created_at "
    "FROM plans WHERE target_date = '{}' ORDER BY plan_type", target_date);
  db::Statement stmt(conn_.handle(), sql);
  while (stmt.step()) {
    Plan p;
    p.id = stmt.column_int64(0);
    p.subject_id = stmt.column_int64(1);
    p.plan_type = stmt.column_text(2);
    p.target_date = stmt.column_text(3);
    p.target_minutes = stmt.column_int64(4);
    p.description = stmt.column_text(5);
    p.is_completed = stmt.column_int64(6) != 0;
    p.created_at = stmt.column_text(7);
    result.push_back(std::move(p));
  }
  return result;
}

std::vector<Plan> PlanRepo::find_all() {
  std::vector<Plan> result;
  db::Statement stmt(conn_.handle(),
    "SELECT id, subject_id, plan_type, target_date, target_minutes, description, is_completed, created_at "
    "FROM plans ORDER BY target_date DESC LIMIT 100");
  while (stmt.step()) {
    Plan p;
    p.id = stmt.column_int64(0);
    p.subject_id = stmt.column_int64(1);
    p.plan_type = stmt.column_text(2);
    p.target_date = stmt.column_text(3);
    p.target_minutes = stmt.column_int64(4);
    p.description = stmt.column_text(5);
    p.is_completed = stmt.column_int64(6) != 0;
    p.created_at = stmt.column_text(7);
    result.push_back(std::move(p));
  }
  return result;
}

void PlanRepo::update(const Plan& plan) {
  db::Statement stmt(conn_.handle(),
    "UPDATE plans SET subject_id = ?, plan_type = ?, target_date = ?, target_minutes = ?, "
    "description = ?, is_completed = ? WHERE id = ?");
  stmt.bind_int(1, plan.subject_id);
  stmt.bind_text(2, plan.plan_type);
  stmt.bind_text(3, plan.target_date);
  stmt.bind_int(4, plan.target_minutes);
  stmt.bind_text(5, plan.description);
  stmt.bind_int(6, plan.is_completed ? 1 : 0);
  stmt.bind_int(7, plan.id);
  stmt.step();
}

void PlanRepo::remove(int64_t id) {
  db::Statement stmt(conn_.handle(), "DELETE FROM plans WHERE id = ?");
  stmt.bind_int(1, id);
  stmt.step();
}

int64_t PlanRepo::create_block(const PlanBlock& block) {
  db::Statement stmt(conn_.handle(),
    "INSERT INTO plan_blocks (plan_id, subject_id, start_time, end_time, day_of_week) "
    "VALUES (?, ?, ?, ?, ?)");
  stmt.bind_int(1, block.plan_id);
  stmt.bind_int(2, block.subject_id);
  stmt.bind_text(3, block.start_time);
  stmt.bind_text(4, block.end_time);
  stmt.bind_int(5, block.day_of_week);
  stmt.step();
  return conn_.last_insert_rowid();
}

std::vector<PlanBlock> PlanRepo::find_blocks_by_plan(int64_t plan_id) {
  std::vector<PlanBlock> result;
  db::Statement stmt(conn_.handle(),
    "SELECT id, plan_id, subject_id, start_time, end_time, day_of_week, created_at "
    "FROM plan_blocks WHERE plan_id = ? ORDER BY start_time");
  stmt.bind_int(1, plan_id);
  while (stmt.step()) {
    PlanBlock b;
    b.id = stmt.column_int64(0);
    b.plan_id = stmt.column_int64(1);
    b.subject_id = stmt.column_int64(2);
    b.start_time = stmt.column_text(3);
    b.end_time = stmt.column_text(4);
    b.day_of_week = static_cast<int>(stmt.column_int64(5));
    b.created_at = stmt.column_text(6);
    result.push_back(std::move(b));
  }
  return result;
}

void PlanRepo::update_block(const PlanBlock& block) {
  db::Statement stmt(conn_.handle(),
    "UPDATE plan_blocks SET start_time = ?, end_time = ?, day_of_week = ? WHERE id = ?");
  stmt.bind_text(1, block.start_time);
  stmt.bind_text(2, block.end_time);
  stmt.bind_int(3, block.day_of_week);
  stmt.bind_int(4, block.id);
  stmt.step();
}

void PlanRepo::remove_block(int64_t block_id) {
  db::Statement stmt(conn_.handle(), "DELETE FROM plan_blocks WHERE id = ?");
  stmt.bind_int(1, block_id);
  stmt.step();
}

} // namespace study_planner
