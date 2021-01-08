cmake_minimum_required(VERSION 3.14)
project(posixcpptimer)

set(CMAKE_CXX_STANDARD 17)
set(BUILD_SHARED_LIBS YES)

# Add the cmake folder so the FindSphinx module is found
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})  

include_directories(include)
include_directories(src)

add_subdirectory(tests)
add_subdirectory(docs)

add_library(${CMAKE_PROJECT_NAME} SHARED
  src/timer.cpp
  include/timer.h
  src/timer_.cpp
  src/timer_.h
  )

target_link_libraries(${CMAKE_PROJECT_NAME} rt)


