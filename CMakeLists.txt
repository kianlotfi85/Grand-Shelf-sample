cmake_minimum_required(VERSION 3.20)
project(study-planner VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# SQLite3
find_package(SQLite3 REQUIRED)

# Source files
set(SOURCES
  src/main.cpp
  src/app/application.cpp
  src/app/config.cpp
  src/db/connection.cpp
  src/db/migrations.cpp
  src/db/statement.cpp
  src/repositories/subject_repo.cpp
  src/repositories/task_repo.cpp
  src/repositories/session_repo.cpp
  src/repositories/study_set_repo.cpp
  src/repositories/plan_repo.cpp
  src/services/task_service.cpp
  src/services/timer_service.cpp
  src/services/analytics_service.cpp
  src/services/planner_service.cpp
  src/ui/console.cpp
  src/ui/commands/task_commands.cpp
  src/ui/commands/timer_commands.cpp
  src/ui/commands/analytics_commands.cpp
  src/ui/commands/planner_commands.cpp
  src/ui/formatters/table_formatter.cpp
)

add_executable(study-planner ${SOURCES})

target_include_directories(study-planner PRIVATE
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/third_party
  ${SQLite3_INCLUDE_DIRS}
)

target_link_libraries(study-planner PRIVATE
  SQLite::SQLite3
  pthread
)

configure_file(
  ${CMAKE_SOURCE_DIR}/config/default_config.json
  ${CMAKE_BINARY_DIR}/config/default_config.json
  COPYONLY
)
