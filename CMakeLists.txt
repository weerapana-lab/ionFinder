
cmake_minimum_required(VERSION 3.9)
project(ionFinder VERSION 0.2.1)

set(LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)

option(BUILD_UTILS "Enable building utils lib" ON)
option(BUILD_ION_FINDER "Enable building ionFinder" ON)
option(BUILD_VISUALIZATION_SCRIPTS "Build and configure scripts to make pdfs of ms2 spectra" ON)

if(BUILD_UTILS)
	list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/utils/cmake")
	list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/utils/thirdparty")
	set(ZLIB_ROOT "${CMAKE_CURRENT_BINARY_DIR}/utils")
	set(ZLIB_VERSION "1.2.11")
	set(ZLIB_URL ${CMAKE_CURRENT_SOURCE_DIR}/utils/thirdparty/zlib1211.zip)
	set(ZLIB_SRC_BASE "${CMAKE_CURRENT_BINARY_DIR}/zlib-${ZLIB_VERSION}")
	include(zlib)

    message("!!! project zlib paths !!!")
    message("${ZLIB_LIBRARIES}")
    message("${ZLIB_INCLUDE_DIRS}")

	add_library(utils INTERFACE)
    set(UTILS_VERSION "1.3.0")

	include(ExternalProject)
	include(GNUInstallDirs)

    set(UTILS_PREFIX "utils-${UTILS_VERSION}")

    ExternalProject_Add(${UTILS_PREFIX}
		SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/utils
		BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/utils
		CMAKE_ARGS "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}"
			"-DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}"
			"-DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}"
			"-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
			"-DCMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}"
			"-DCMAKE_SYSTEM_PROCESSOR=${CMAKE_SYSTEM_PROCESSOR}"
			"-DCMAKE_AR=${CMAKE_AR}"
			"-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}"
			"-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
			"-DCMAKE_FIND_ROOT_PATH=${CMAKE_FIND_ROOT_PATH}"
			"-DZLIB_FOUND:BOOL=TRUE"
            "-DZLIB_LIBRARIES=${ZLIB_LIBRARIES}"
            "-DZLIB_INCLUDE_DIRS=${ZLIB_INCLUDE_DIRS}"
		)

    add_dependencies(${UTILS_PREFIX} ${ZLIB_PREFIX})
    add_dependencies(utils ${UTILS_PREFIX})
    set(UTILS_LIBRARY ${LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_STATIC_LIBRARY_PREFIX}utils${CMAKE_STATIC_LIBRARY_SUFFIX})
    set(UTILS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/utils/include)
endif()

if(BUILD_ION_FINDER)
	add_subdirectory("ionFinder")
endif()

if(BUILD_VISUALIZATION_SCRIPTS MATCHES ON)
	add_subdirectory("python")
endif()

