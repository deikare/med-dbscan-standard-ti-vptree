cmake_minimum_required(VERSION 3.1...3.25)
set (CMAKE_CXX_STANDARD 17)
# for testing change build type from Debug to Release
set(CMAKE_BUILD_TYPE Debug) 
add_compile_options(-Wall -Wextra -pedantic -Werror)

set(PROJECT DBSCAN)
set(CPP_DIR src)

project(
  ${PROJECT}
  VERSION 1.0
  LANGUAGES CXX)

add_executable(${PROJECT} ${CPP_DIR}/main.cpp
                          ${CPP_DIR}/parser.cpp
                          ${CPP_DIR}/reader.cpp
                          ${CPP_DIR}/math.cpp
                          ${CPP_DIR}/vp_tree.cpp
                          ${CPP_DIR}/analyzer.cpp)

target_include_directories(${PROJECT} PRIVATE include)

