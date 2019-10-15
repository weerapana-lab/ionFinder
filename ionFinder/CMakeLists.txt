cmake_minimum_required(VERSION 3.15)
project(ionFinder)

SET(CMAKE_BUILD_TYPE Debug)
set(ION_FINDER_TARGET ionFinder)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

set(CMAKE_CXX_STANDARD 11)

#file(GLOB COMMON_SOURCE_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)
#file(GLOB ION_FINDER_SRC ${PROJECT_SOURCE_DIR}/src/ionFinder/*.cpp)
#set(ION_FINDER_SRC "${ION_FINDER_SRC} ${COMMON_SOURCE_FILES}")

add_executable(${ION_FINDER_TARGET}
        src/ionFinder/main.cpp
        src/scanData.cpp
        src/geometry.cpp
        src/calcLableLocs.cpp
        src/aaDB.cpp
        src/sequestParams.cpp
        src/dtafilter.cpp
        src/paramsBase.cpp
        src/peptide.cpp
        src/ms2Spectrum.cpp
        src/ms2.cpp
        src/ionFinder/datProc.cpp
        src/ionFinder/inputFiles.cpp
        src/ionFinder/params.cpp)

add_custom_target(
        config_hpp ALL
        COMMAND ${CMAKE_SOURCE_DIR}/configure -d ${CMAKE_SOURCE_DIR}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_dependencies(${ION_FINDER_TARGET} config_hpp)

target_include_directories(${ION_FINDER_TARGET} PUBLIC ${CMAKE_SOURCE_DIR}/include)

#add utils library
include(FindPkgConfig)
find_package(utils REQUIRED)
target_include_directories(${ION_FINDER_TARGET} PUBLIC ${UTILS_INCLUDE_DIR})
target_link_libraries(${ION_FINDER_TARGET} ${UTILS_LIB})

#add thread library
find_package(Threads)
target_link_libraries(${ION_FINDER_TARGET} ${CMAKE_THREAD_LIBS_INIT})

