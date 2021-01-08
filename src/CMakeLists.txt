
set(CMAKE_CXX_STANDARD 17)
set(BUILD_SHARED_LIBS YES)

include_directories(../include)


add_library(timer SHARED
        timer.cpp
        ../include/timer.h
        timer_.cpp
        timer_.h
        )
target_link_libraries(timer rt)

