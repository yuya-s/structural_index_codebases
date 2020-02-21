cmake_minimum_required(VERSION 3.7)
project(construct_wapathindex)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        graph.cpp
        graph.h
        index.cpp
        index.h
        main.cpp
        pathset.cpp
        pathset.h
        utility.h
        workload.cpp
        workload.h)

add_executable(construct_wapathindex ${SOURCE_FILES})