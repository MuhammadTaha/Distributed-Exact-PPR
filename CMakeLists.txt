cmake_minimum_required(VERSION 3.14)
project(thesis)

set(CMAKE_CXX_STANDARD 14)

include_directories(cmake-build-debug)

add_executable(
        comm.cpp
        comm.h
        construct.cpp
        construct.h
        graph.cpp
        graph.h
        precompute.cpp
        precompute.h
        rwr.cpp
        rwr.h
        utility.cpp
        utility.h
        main.cpp
        )
