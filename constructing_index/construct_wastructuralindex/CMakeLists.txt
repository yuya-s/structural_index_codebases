cmake_minimum_required(VERSION 3.7)
project(construct_wastrtucturalindex)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        graph.cpp
        graph.h
        index.cpp
        index.h
        main.cpp
        utility.h
        workload.cpp
        workload.h
        workloadaware_transequivalent.cpp
        workloadaware_transequivalent.h)

add_executable(construct_wastrtucturalindex ${SOURCE_FILES})