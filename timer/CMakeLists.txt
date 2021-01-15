add_library(${CMAKE_PROJECT_NAME}-timer SHARED
  ../include/timer.h
  timer.cpp
  timer_.cpp
  timer_.h
  )

target_link_libraries(${CMAKE_PROJECT_NAME}-timer rt)
