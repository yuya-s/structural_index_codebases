cmake_minimum_required(VERSION 3.7)
project(construct_structuralindex)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        graph.cpp
        graph.h
        index.cpp
        index.h
        kbisimulation.cpp
        kbisimulation.h
        main.cpp
        utility.h)

add_executable(construct_structuralindex ${SOURCE_FILES})