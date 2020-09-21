# required cmake version
cmake_minimum_required(VERSION 3.0)

# project name
project(sqliteApp)

# compiler options
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable(
  # application name
  sqliteApp

  # source files
  src/main.cpp
  src/Sqlite.cpp
)

# include and lib paths
include_directories(/usr/include)
link_directories(/usr/lib)

# link sqlite
target_link_libraries(sqliteApp sqlite3)