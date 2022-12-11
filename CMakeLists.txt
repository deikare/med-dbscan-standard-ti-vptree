cmake_minimum_required(VERSION 3.1...3.25)

set(PROJECT DBSCAN)
set(CPP_DIR src)

project(
  ${PROJECT}
  VERSION 1.0
  LANGUAGES CXX)

add_executable(${PROJECT} ${CPP_DIR}/main.cpp
                          ${CPP_DIR}/parser.cpp)

target_include_directories(${PROJECT} PRIVATE include)

