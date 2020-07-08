cmake_minimum_required(VERSION 3.9)
project(ionFinder VERSION 0.2.1)

option(BUILD_UTILS "Enable building utils lib" ON)
option(BUILD_ION_FINDER "Enable building ionFinder" ON)

if(BUILD_UTILS)
	add_subdirectory("utils")
endif()

if(BUILD_ION_FINDER)
	add_subdirectory("ionFinder")
endif()

