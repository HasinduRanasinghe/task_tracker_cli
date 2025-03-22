cmake_minimum_required(VERSION 3.10)
project(TaskTracker VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add executable
add_executable(tasktracker task_tracker.cpp)

# Install target
install(TARGETS tasktracker DESTINATION bin)