cmake_minimum_required(VERSION 3.7)
project(query_bfs)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        graph.cpp
        graph.h
        index.cpp
        index.h
        main.cpp
        query.cpp
        query.h
        utility.h)

add_executable(query_bfs ${SOURCE_FILES})