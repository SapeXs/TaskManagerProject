cmake_minimum_required(VERSION 3.20)

project(TaskManagerProject LANGUAGES CXX)

find_package(PkgConfig QUIET)

if(PkgConfig_FOUND)
  pkg_check_modules(LIBNOTIFY QUIET libnotify)
endif()

if(LIBNOTIFY_FOUND)
  message(STATUS "libnotify found: notifications enabled")
  message(STATUS "libnotify include dirs: ${LIBNOTIFY_INCLUDE_DIRS}")
  message(STATUS "libnotify libraries: ${LIBNOTIFY_LIBRARIES}")
else()
  message(WARNING "libnotify not found: notifications disabled. Install libnotify-dev to enable them.")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Threads REQUIRED)

option(TASKMANAGER_ENABLE_WARNINGS "Enable compiler warnings" ON)
option(TASKMANAGER_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

add_library(task_manager_project_options INTERFACE)
target_compile_features(task_manager_project_options INTERFACE cxx_std_20)

if(TASKMANAGER_ENABLE_WARNINGS)
  target_compile_options(
    task_manager_project_options
    INTERFACE
      $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic -Wshadow -Wnon-virtual-dtor -Wold-style-cast -Woverloaded-virtual>
      $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
  )
endif()

if(TASKMANAGER_WARNINGS_AS_ERRORS)
  target_compile_options(
    task_manager_project_options
    INTERFACE
      $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Werror>
      $<$<CXX_COMPILER_ID:MSVC>:/WX>
  )
endif()

add_library(
  task_manager_core
  src/app/app_paths.cpp
  src/core/task_base.cpp
  src/core/task_converters.cpp
  src/core/duration_parser.cpp
  src/core/date_time_utils.cpp
  src/manager/task_manager.cpp
  src/storage/task_storage.cpp
  src/storage/storage_codec.cpp
  src/storage/task_constructor.cpp
  src/tasks/recurring_task.cpp
  src/tasks/bounded_recurring_task.cpp
  src/tasks/reminder_task.cpp
  src/tasks/savings_task.cpp
  src/tasks/stepped_deadline_task.cpp
  src/tasks/final_deadline_task.cpp
  src/commands/command.cpp
  src/commands/command_parser.cpp
  src/commands/command_type.cpp
  src/format/task_formatter.cpp
  src/progress/time_left_progress.cpp
)

target_include_directories(task_manager_core PUBLIC include)
target_link_libraries(task_manager_core PUBLIC task_manager_project_options)

add_executable(
  taskmanagerd
  apps/taskmanagerd.cpp
  src/app/daemon_app.cpp
  src/app/command_handler.cpp
  src/ipc/unix_socket_server.cpp
  src/notifications/notification_service.cpp
)

target_link_libraries(taskmanagerd
    PRIVATE
        task_manager_core
        task_manager_project_options
        Threads::Threads
)

if(LIBNOTIFY_FOUND)
  target_compile_definitions(taskmanagerd PRIVATE TASKMANAGER_ENABLE_NOTIFICATIONS)
  target_include_directories(taskmanagerd PRIVATE ${LIBNOTIFY_INCLUDE_DIRS})
  target_link_libraries(taskmanagerd PRIVATE ${LIBNOTIFY_LIBRARIES})
  target_compile_options(taskmanagerd PRIVATE ${LIBNOTIFY_CFLAGS_OTHER})
endif()

add_executable(
  taskctl
  apps/taskctl.cpp
  src/ipc/unix_socket_client.cpp
)

target_include_directories(taskctl PRIVATE include)
target_link_libraries(taskctl PRIVATE task_manager_core task_manager_project_options)

file(
  GLOB_RECURSE
  TASKMANAGER_FORMAT_FILES
  CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/apps/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/*.tpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

file(
  GLOB_RECURSE
  TASKMANAGER_TIDY_FILES
  CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/apps/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

find_program(CLANG_FORMAT_EXE NAMES clang-format)
if(CLANG_FORMAT_EXE)
  add_custom_target(
    format
    COMMAND ${CLANG_FORMAT_EXE} -i ${TASKMANAGER_FORMAT_FILES}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Formatting C++ sources with clang-format"
  )

  add_custom_target(
    check-format
    COMMAND ${CLANG_FORMAT_EXE} --dry-run --Werror ${TASKMANAGER_FORMAT_FILES}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Checking C++ formatting with clang-format"
  )
else()
  add_custom_target(
    format
    COMMAND ${CMAKE_COMMAND} -E echo "clang-format not found. Install clang-format or use scripts/format_cpp.sh after installation."
  )

  add_custom_target(
    check-format
    COMMAND ${CMAKE_COMMAND} -E echo "clang-format not found. Install clang-format to check formatting."
  )
endif()

find_program(CLANG_TIDY_EXE NAMES clang-tidy)
if(CLANG_TIDY_EXE)
  add_custom_target(
    tidy
    COMMAND ${CLANG_TIDY_EXE} -p ${CMAKE_BINARY_DIR} ${TASKMANAGER_TIDY_FILES}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running clang-tidy"
  )
else()
  add_custom_target(
    tidy
    COMMAND ${CMAKE_COMMAND} -E echo "clang-tidy not found. Install clang-tidy or use scripts/tidy_cpp.sh after installation."
  )
endif()

add_custom_target(install-system-deps
    COMMAND ${CMAKE_SOURCE_DIR}/scripts/install_deps_ubuntu.sh
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Installing Ubuntu/Debian system dependencies"
)
