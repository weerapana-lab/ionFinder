cmake_minimum_required(VERSION 3.9)
project(ionFinder VERSION 1.0.2)

SET(CMAKE_BUILD_TYPE Debug)
set(ION_FINDER_TARGET ionFinder)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FindPkgConfig)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../peptideUtils/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(tsv_constants) #defines tsv file column headers for help file.

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

set(PROG_WD_DIR "${PROJECT_SOURCE_DIR}" CACHE STRING "Path to program source parent directory.")
set(PROG_SHARE_DIR "${CMAKE_CURRENT_BINARY_DIR}/share" CACHE STRING "Path directory with aa masses and default smod file.")
set(GIT_PRE_CONFIGURE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/git_info.h.in)
set(GIT_POST_CONFIGURE_FILE ${CMAKE_CURRENT_BINARY_DIR}/include/git_info.h)

add_executable(${ION_FINDER_TARGET}
        src/ionFinder/main.cpp
        src/scanData.cpp
        src/geometry.cpp
        src/statistics.cpp
        src/calcLableLocs.cpp
        src/aaDB.cpp
        src/sequestParams.cpp
        src/dtafilter.cpp
        src/paramsBase.cpp
        src/peptide.cpp
        src/ms2Spectrum.cpp
        src/ionFinder/datProc.cpp
        src/ionFinder/inputFiles.cpp
        src/ionFinder/params.cpp
		src/msInterface.cpp)

target_include_directories(${ION_FINDER_TARGET}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
        PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/include)

#add peptideUtils library
if(NOT DEFINED PEPTIDE_UTILS_INCLUDE_DIR OR NOT DEFINED PEPTIDE_UTILS_LIBRARY)
	find_package(peptideUtils REQUIRED)
endif()
target_include_directories(${ION_FINDER_TARGET} PUBLIC ${PEPTIDE_UTILS_INCLUDE_DIR})
target_link_libraries(${ION_FINDER_TARGET} ${PEPTIDE_UTILS_LIBRARY})

# add zlib library
option(ENABLE_ZLIB "Link to zlib?" ON)
if(${ENABLE_ZLIB})
	set(ZLIB_ROOT "${CMAKE_BINARY_DIR}" CACHE STRING "Path to zlib installation")
	if(NOT ZLIB_FOUND)
		find_package(zlib REQUIRED)
	endif()
    message("-- ionFinder zlib shared lib ${ZLIB_LIBRARIES}")
    message("-- ionFinder zlib include dir ${ZLIB_INCLUDE_DIRS}")
	target_include_directories(${ION_FINDER_TARGET} PUBLIC ${ZLIB_INCLUDE_DIRS})
    target_link_libraries(${ION_FINDER_TARGET} ${ZLIB_LIBRARIES})
endif()

#add auto generated headers
find_package(Python REQUIRED)
if(PYTHON_VERSION VERSION_LESS 2.7.0)
       message(FATAL_ERROR "python >= 2.7 is required!")
endif()
set(SRC_ETC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../etc)
set(SRC_SCRIPTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../scripts)
set(.SPEC_CONSTANTS_NAME spectrum_constants)
macro(add_configure_file name target_name output_dir extension)
add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${output_dir}/${name}.${extension}
        DEPENDS
            ${SRC_ETC_DIR}/${name}.config
            ${SRC_ETC_DIR}/${name}.${extension}.in
        COMMENT "Configuring ${name}..."
        COMMAND
            ${SRC_SCRIPTS_DIR}/configure_file
            ${SRC_ETC_DIR}/${name}.config
            ${SRC_ETC_DIR}/${name}.${extension}.in
            ${CMAKE_CURRENT_BINARY_DIR}/${output_dir}/${name}.${extension}
        )
add_custom_target(${target_name} ALL
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${output_dir}/${name}.${extension}
        )
endmacro()
add_configure_file(spectrum_constants SpectrumConstantsHpp "include" "hpp")
add_dependencies(${ION_FINDER_TARGET} SpectrumConstantsHpp)
add_configure_file(tsv_constants TSVConstantsHpp "include" "hpp")
add_dependencies(${ION_FINDER_TARGET} TSVConstantsHpp)

#add thread library
find_package(Threads)
target_link_libraries(${ION_FINDER_TARGET} ${CMAKE_THREAD_LIBS_INIT})

#configure share dir
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/share/staticModifications.txt
        ${PROG_SHARE_DIR}/staticModifications.txt COPYONLY)

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
			set(BUILD_MAN_PDF_OUT ${CMAKE_CURRENT_BINARY_DIR}/doc/update_${target}_help.sh)
            set(TARGET_MAN_PDF_IN ${PROG_MAN_DIR}/${target}/helpFile.roff)
			set(TARGET_MAN_PDF_OUT ${CMAKE_CURRENT_BINARY_DIR}/doc/${target}_helpFile.pdf)

            file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc)

            configure_file(${BUILD_MAN_PDF_IN} ${BUILD_MAN_PDF_OUT} @ONLY)

            add_custom_command(
                    OUTPUT ${TARGET_MAN_PDF_OUT}
                    COMMAND sh ${BUILD_MAN_PDF_OUT}
                    DEPENDS ${BUILD_MAN_PDF_IN} ${TARGET_MAN_PDF_IN}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc
                    COMMENT "Generating ${TARGET_MAN_PDF_OUT}"
                    VERBATIM)
            add_custom_target(${target}_help_pdf ALL
                    DEPENDS ${TARGET_MAN_PDF_OUT}
                    )

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

