cmake_minimum_required(VERSION 3.7)
project(construct_pathindex)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        graph.cpp
        graph.h
        index.cpp
        index.h
        main.cpp
        pathset.cpp
        pathset.h
        utility.h)

add_executable(construct_pathindex ${SOURCE_FILES})