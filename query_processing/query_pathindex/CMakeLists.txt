cmake_minimum_required(VERSION 3.7)
project(query_pathindex)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        index.cpp
        index.h
        main.cpp
        query.cpp
        query.h
        utility.h)

add_executable(query_pathindex ${SOURCE_FILES})