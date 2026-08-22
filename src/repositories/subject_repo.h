#pragma once
#include "db/connection.h"
#include "models/subject.h"
#include <vector>
#include <optional>

namespace study_planner {

class SubjectRepo {
public:
  explicit SubjectRepo(db::Connection& conn) : conn_(conn) {}

  int64_t create(const Subject& subject);
  std::optional<Subject> find_by_id(int64_t id);
  std::optional<Subject> find_by_name(const std::string& name);
  std::vector<Subject> find_all();
  void update(const Subject& subject);
  void remove(int64_t id);

private:
  db::Connection& conn_;
};

} // namespace study_planner
