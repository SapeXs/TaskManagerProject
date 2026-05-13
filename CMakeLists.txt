cmake_minimum_required(VERSION 3.20)

project(TaskManagerProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(task_manager_core
    src/core/task_base.cpp
    src/manager/task_manager.cpp

    src/tasks/recurring_task.cpp
    src/tasks/bounded_recurring_task.cpp
    src/tasks/reminder_task.cpp

    src/tasks/savings_task.cpp
    src/tasks/stepped_deadline_task.cpp
    src/tasks/final_deadline_task.cpp

    src/storage/task_storage.cpp
)

target_include_directories(task_manager_core PUBLIC include)

add_executable(taskmanagerd
    apps/taskmanagerd.cpp
    src/app/daemon_app.cpp
)

target_link_libraries(taskmanagerd PRIVATE task_manager_core)