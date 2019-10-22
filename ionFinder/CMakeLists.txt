cmake_minimum_required(VERSION 3.9)
project(ionFinder VERSION 1.0.1)

SET(CMAKE_BUILD_TYPE Debug)
set(ION_FINDER_TARGET ionFinder)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

set(CMAKE_CXX_STANDARD 11)

#file(GLOB COMMON_SOURCE_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)
#file(GLOB ION_FINDER_SRC ${PROJECT_SOURCE_DIR}/src/ionFinder/*.cpp)
#set(ION_FINDER_SRC "${ION_FINDER_SRC} ${COMMON_SOURCE_FILES}")

set(PROG_WD_DIR "${PROJECT_SOURCE_DIR}" CACHE STRING "Path to directory with defaultResidueAtoms.txt")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPROG_WD_DIR=\\\"${PROG_WD_DIR}\\\"")

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

target_include_directories(${ION_FINDER_TARGET} PUBLIC ${CMAKE_SOURCE_DIR}/include)

#add utils library
include(FindPkgConfig)
find_package(utils REQUIRED)
target_include_directories(${ION_FINDER_TARGET} PUBLIC ${UTILS_INCLUDE_DIR})
target_link_libraries(${ION_FINDER_TARGET} ${UTILS_LIB})

#add thread library
find_package(Threads)
target_link_libraries(${ION_FINDER_TARGET} ${CMAKE_THREAD_LIBS_INIT})

option(BUILD_DOC "Build documentation" OFF)
if(BUILD_DOC MATCHES ON)
# check if Doxygen is installed
find_package(Doxygen)
if (DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/doxygen/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile)

    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    # note the option ALL which allows to build the docs together with the application
    add_custom_target(doxygen ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM )
else (DOXYGEN_FOUND)
    message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)
endif()

option(BUILD_MAN_PDF "Build pdf of man page" ON)
if(BUILD_MAN_PDF MATCHES ON)

    macro(build_man_page target)
        find_package(groff REQUIRED)
        find_package(ps2pdf REQUIRED)
        if(${GROFF_FOUND} AND ${PS2PDF_FOUND})
            set(BUILD_MAN_PDF_IN ${CMAKE_CURRENT_SOURCE_DIR}/man/updateMan.in)
            set(BUILD_MAN_PDF_OUT ${CMAKE_BINARY_DIR}/doc/update_${target}_help.sh)
            set(TARGET_MAN_PDF_IN ${CMAKE_CURRENT_SOURCE_DIR}/man/${target}/helpFile.roff)
            set(TARGET_MAN_PDF_OUT ${CMAKE_BINARY_DIR}/doc/${target}_helpFile.pdf)

            file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc)

            configure_file(${BUILD_MAN_PDF_IN} ${BUILD_MAN_PDF_OUT} @ONLY)

            add_custom_target(${target}_help_pdf ALL
                    COMMAND sh ${BUILD_MAN_PDF_OUT}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc
                    COMMENT "Generating ${TARGET_MAN_PDF_OUT}"
                    VERBATIM)

       else()
           if(${GROFF_NOT_FOUND})
               message(WARNING "groff not found!")
           endif()
           if(${PS2PDF_NOT_FOUND})
               message(WARNING "ps2pdf not found!")
           endif()
           message("Could not build pdf man page!")
        endif()
    endmacro()

    build_man_page(${ION_FINDER_TARGET})
    #build_man_page(ms2_annotator)
endif ()
