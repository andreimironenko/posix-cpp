cmake_minimum_required(VERSION 3.14)
project(posix-cpp-timer)

set(CMAKE_CXX_STANDARD 17)
set(BUILD_SHARED_LIBS YES)

include_directories(include)
include_directories(src)

add_subdirectory(tests)

add_library(${CMAKE_PROJECT_NAME} SHARED
  src/timer.cpp
  include/timer.h
  src/timer_.cpp
  src/timer_.h
  )

target_link_libraries(${CMAKE_PROJECT_NAME} rt)


