
cmake_minimum_required(VERSION 3.9)
project(ionFinder VERSION 1.0.2)

set(LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)

option(BUILD_PEPTIDE_UTILS "Enable building peptideUtils lib" ON)
option(BUILD_ION_FINDER "Enable building ionFinder" ON)
option(BUILD_VISUALIZATION_SCRIPTS "Build and configure scripts to make pdfs of ms2 spectra" ON)

if(BUILD_PEPTIDE_UTILS)
	list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/peptideUtils/cmake")
	list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/peptideUtils/thirdparty")

    option(SYSTEM_ZLIB "Use system zlib library" ON)
	option(ENABLE_ZLIB "Add support for zlib decompression" ON)

	if(${ENABLE_ZLIB})
		set(ZLIB_ROOT "${CMAKE_CURRENT_BINARY_DIR}/peptideUtils")
		set(ZLIB_VERSION "1.2.11")
		set(ZLIB_URL ${CMAKE_CURRENT_SOURCE_DIR}/peptideUtils/thirdparty/zlib1211.zip)
		set(ZLIB_SRC_BASE "${CMAKE_CURRENT_BINARY_DIR}/zlib-${ZLIB_VERSION}")
		include(zlib)

        message("-- Project zlib shared lib ${ZLIB_LIBRARIES}")
        message("-- Project zlib include dir ${ZLIB_INCLUDE_DIRS}")
		set(PEPTIDE_UTILS_CMAKE_ARGS "-DENABLE_ZLIB=ON"
							 "-DZLIB_FOUND:BOOL=TRUE"
							 "-DZLIB_LIBRARIES=${ZLIB_LIBRARIES}"
							 "-DZLIB_INCLUDE_DIRS=${ZLIB_INCLUDE_DIRS}")
	else()
		set(PEPTIDE_UTILS_CMAKE_ARGS "-DENABLE_ZLIB=OFF")
	endif()

	add_library(peptideUtils INTERFACE)
	set(PEPTIDE_UTILS_VERSION "1.3.0")

	include(ExternalProject)
	include(GNUInstallDirs)

	set(PEPTIDE_UTILS_PREFIX "peptideUtils-${PEPTIDE_UTILS_VERSION}")

	ExternalProject_Add(${PEPTIDE_UTILS_PREFIX}
		SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/peptideUtils
		BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/peptideUtils
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
			"${PEPTIDE_UTILS_CMAKE_ARGS}"
		)

	if(${ENABLE_ZLIB} AND NOT ${SYSTEM_ZLIB})
		add_dependencies(${PEPTIDE_UTILS_PREFIX} ${ZLIB_PREFIX})
	endif()
	add_dependencies(peptideUtils ${PEPTIDE_UTILS_PREFIX})
	set(PEPTIDE_UTILS_LIBRARY ${LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_STATIC_LIBRARY_PREFIX}peptideUtils${CMAKE_STATIC_LIBRARY_SUFFIX})
	set(PEPTIDE_UTILS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/peptideUtils/include)
endif()

if(BUILD_ION_FINDER)
	add_subdirectory("ionFinder")
endif()

if(BUILD_VISUALIZATION_SCRIPTS MATCHES ON)
	set(PYTHON_RUNTIME_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/bin")
	add_subdirectory("python")
endif()

