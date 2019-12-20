cmake_minimum_required(VERSION 3.9)
project(ionFinder VERSION 0.1.1)

SET(CMAKE_BUILD_TYPE Debug)
set(ION_FINDER_TARGET ionFinder)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

set(CMAKE_CXX_STANDARD 11)

if(MSVC)
    # Force to always compile with W4
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    # Update if necessary
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")
endif()

#file(GLOB COMMON_SOURCE_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)
#file(GLOB ION_FINDER_SRC ${PROJECT_SOURCE_DIR}/src/ionFinder/*.cpp)
#set(ION_FINDER_SRC "${ION_FINDER_SRC} ${COMMON_SOURCE_FILES}")

set(PROG_WD_DIR "${PROJECT_SOURCE_DIR}" CACHE STRING "Path to program source parent directory.")
set(PROG_SHARE_DIR "${PROJECT_SOURCE_DIR}/share" CACHE STRING "Path directory with aa masses and default smod file.")
set(GIT_PRE_CONFIGURE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/git_info.h.in)
set(GIT_POST_CONFIGURE_FILE ${CMAKE_CURRENT_BINARY_DIR}/include/git_info.h)

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

target_include_directories(${ION_FINDER_TARGET}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
        PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/include)

#add utils library
include(FindPkgConfig)
find_package(utils REQUIRED)
target_include_directories(${ION_FINDER_TARGET} PUBLIC ${UTILS_INCLUDE_DIR})
target_link_libraries(${ION_FINDER_TARGET} ${UTILS_LIB})

#add auto generated headers
set(SRC_ETC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../etc)
set(SRC_SCRIPTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../scripts)
set(.SPEC_CONSTANTS_NAME spectrum_constants)
add_custom_target(SpectrumConstantsHpp
        DEPENDS
            ${SRC_ETC_DIR}/${.SPEC_CONSTANTS_NAME}.config
            ${SRC_ETC_DIR}/${.SPEC_CONSTANTS_NAME}.hpp.in
        COMMENT "Writing custom ${.SPEC_CONSTANTS_NAME} header..."
        BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/include/${.SPEC_CONSTANTS_NAME}.hpp
        COMMAND
            ${SRC_SCRIPTS_DIR}/configure_file
            ${SRC_ETC_DIR}/${.SPEC_CONSTANTS_NAME}.config
            ${SRC_ETC_DIR}/${.SPEC_CONSTANTS_NAME}.hpp.in
            ${CMAKE_CURRENT_BINARY_DIR}/include/${.SPEC_CONSTANTS_NAME}.hpp
        )
add_dependencies(${ION_FINDER_TARGET} SpectrumConstantsHpp)


#add thread library
find_package(Threads)
target_link_libraries(${ION_FINDER_TARGET} ${CMAKE_THREAD_LIBS_INIT})

#set up cmake generated headers
set(PROG_MAN_DIR ${CMAKE_CURRENT_BINARY_DIR}/man)
set(CONFIG_PRE_CONFIGURE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.h.in)
set(CONFIG_POST_CONFIGURE_FILE ${CMAKE_CURRENT_BINARY_DIR}/include/config.h)
configure_file(${CONFIG_PRE_CONFIGURE_FILE} ${CONFIG_POST_CONFIGURE_FILE} @ONLY)

#configure man page
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/man)
macro(configure_man_troff target)
    file(MAKE_DIRECTORY ${PROG_MAN_DIR}/${target})
    set(MAN_PRE_CONFIGURE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/man/${target}/helpFile.roff.in)
    set(MAN_POST_CONFIGURE_FILE ${PROG_MAN_DIR}/${target}/helpFile.roff)
    set(USAGE_PRE_CONFIGURE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/man/${target}/usage.txt.in)
    set(USAGE_POST_CONFIGURE_FILE ${PROG_MAN_DIR}/${target}/usage.txt)
    string(TIMESTAMP BUILD_TIMESTAMP "%d %b %y")
    configure_file(${MAN_PRE_CONFIGURE_FILE} ${MAN_POST_CONFIGURE_FILE} @ONLY)
    configure_file(${USAGE_PRE_CONFIGURE_FILE} ${USAGE_POST_CONFIGURE_FILE} @ONLY)
endmacro()
configure_man_troff(${ION_FINDER_TARGET})
#configure_man_troff(ms2_annotator)

#git tracking
option(TRACK_GIT "Should git tracking be enabeled?" ON)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include)
if(TRACK_GIT)
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/git_watcher.cmake)
    add_dependencies(${ION_FINDER_TARGET} check_git_repository)
    set_directory_properties(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "git-state")
else()
    function(WriteBlankGitHeader)
        set(GIT_RETRIEVED_STATE "false")
        set(GIT_HEAD_SHA1 "GIT_NOT_FOUND")
        set(GIT_IS_DIRTY "false")
        set(GIT_LAST_COMMIT_DATE "GIT_NOT_FOUND")
        configure_file("${GIT_PRE_CONFIGURE_FILE}" "${GIT_POST_CONFIGURE_FILE}" @ONLY)
    endfunction()
    WriteBlankGitHeader()
endif()

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
            set(TARGET_MAN_PDF_IN ${PROG_MAN_DIR}/${target}/helpFile.roff)
            set(TARGET_MAN_PDF_OUT ${CMAKE_BINARY_DIR}/doc/${target}_helpFile.pdf)

            file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc)

            configure_file(${BUILD_MAN_PDF_IN} ${BUILD_MAN_PDF_OUT} @ONLY)

            add_custom_target(${target}_help_pdf ALL
                    COMMAND sh ${BUILD_MAN_PDF_OUT}
                    DEPENDS ${BUILD_MAN_PDF_IN} ${TARGET_MAN_PDF_IN}
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
endif()

option(BUILDN_VISUALIZATION_SCRIPTS "Build and configure scripts to make pdfs of ms2 spectra" ON)
if(BUILDN_VISUALIZATION_SCRIPTS MATCHES ON)
    find_program(BASH bash DOC "Path to bash interpreter.")
    execute_process(COMMAND which python OUTPUT_VARIABLE FOUND_PYTHON_EXE_PATH)
    string(REGEX REPLACE "\n$" "" FOUND_PYTHON_EXE_PATH "${FOUND_PYTHON_EXE_PATH}")
    set(PYTHON_PATH ${FOUND_PYTHON_EXE_PATH} CACHE STRING "Path to python interpreter to use.")
    set(RUN_MAKE_MS2_SCR_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../python/run_make_ms2.py
            CACHE STRING "Path to run_make_ms2.py")
    set(MAKE_MS2_SRC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../python/make_ms2.py
            CACHE STRING "Path to make_ms2.py")
    set(PARSE_SCAFFOLD_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../python/parse_scaffold.py
            CACHE STRING "Path to parse_scaffold.py")
    set(PYTHON_BUILD_SCRIPT ${SRC_SCRIPTS_DIR}/addPythonFiles.sh
            CACHE STRING "Path to make_ms2.py")

    add_custom_target(make_ms2
            ALL
            DEPENDS ${MAKE_MS2_SRC_PATH}
            COMMENT "Adding make_ms2 executable."
            COMMAND
                ${BASH} ${PYTHON_BUILD_SCRIPT}
                --pythonExe ${PYTHON_PATH}
                --mainArgs ${CMAKE_CURRENT_SOURCE_DIR}/../rscripts/makeMs2.R
                ${MAKE_MS2_SRC_PATH}
            )

    add_custom_target(run_make_ms2
            ALL
            DEPENDS ${RUN_MAKE_MS2_SCR_PATH}
            COMMENT "Adding run_make_ms2 executable."
            COMMAND
                ${BASH} ${PYTHON_BUILD_SCRIPT}
                --pythonExe ${PYTHON_PATH}
                ${RUN_MAKE_MS2_SCR_PATH}
            )

    add_custom_target(parse_scaffold
            ALL
            DEPENDS ${PARSE_SCAFFOLD_PATH}
            COMMENT "Adding parse_scaffold executable."
            COMMAND
            ${BASH} ${PYTHON_BUILD_SCRIPT}
            --pythonExe ${PYTHON_PATH}
            ${PARSE_SCAFFOLD_PATH}
            )

endif()
