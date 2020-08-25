
cmake_minimum_required(VERSION 3.9)
project(ionFinder VERSION 0.2.1)

set(LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)

option(BUILD_UTILS "Enable building utils lib" ON)
option(BUILD_ION_FINDER "Enable building ionFinder" ON)

macro(add_external_dependency name version cmake_prefix)
	add_library(${name} INTERFACE)
	set(${cmake_prefix}_VERSION ${version})

	include(ExternalProject)
	include(GNUInstallDirs)

	set(${cmake_prefix}_PREFIX "${name}-${${cmake_prefix}_VERSION}")

	ExternalProject_Add(${${cmake_prefix}_PREFIX}
		SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${name}
		BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/${name}
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
		)

	add_dependencies(${name} ${${cmake_prefix}_PREFIX})
	set(${cmake_prefix}_LIBRARY ${LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_STATIC_LIBRARY_PREFIX}${name}${CMAKE_STATIC_LIBRARY_SUFFIX})
	set(${cmake_prefix}_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${name}/include)
endmacro()

if(BUILD_UTILS)
	add_external_dependency(utils "1.3.0" UTILS)
endif()

if(BUILD_ION_FINDER)
	set(PYTHON_RUNTIME_OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin)
	add_subdirectory("ionFinder")
endif()

