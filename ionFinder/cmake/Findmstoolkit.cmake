
set(FIND_MSTOOLKIT_PATHS
		${CMAKE_BINARY_DIR}/lib
		${CMAKE_BINARY_DIR}/mstoolkit/lib
		${CMAKE_CURRENT_BINARY_DIR}/../mstoolkit
		${CMAKE_CURRENT_BINARY_DIR}/../build/mstoolkit
		${CMAKE_CURRENT_BINARY_DIR}/../mstoolkit_build
		${CMAKE_CURRENT_SOURCE_DIR}/../lib
		${CMAKE_CURRENT_SOURCE_DIR}/../mstoolkit
		${CMAKE_CURRENT_SOURCE_DIR}/../mstoolkit/lib
		${CMAKE_CURRENT_SOURCE_DIR}/lib
		${CMAKE_CURRENT_SOURCE_DIR}/mstoolkit
        /usr/local
        /usr/local/lib)

	find_path(MSTOOLKIT_INCLUDE_DIR
		MSToolkitTypes.h
		Spectrum.h
		MSReader.h
        PATH_SUFFIXES include
		PATHS ${FIND_MSTOOLKIT_PATHS})

	find_library(MSTOOLKIT_LIBRARY
		NAMES mstoolkit libmstoolkit.a libmstoolkit ${CMAKE_STATIC_LIBRARY_PREFIX}mstoolkit${CMAKE_STATIC_LIBRARY_SUFFIX}
		PATHS ${FIND_MSTOOLKIT_PATHS}
		DOC "Path to mstoolkit library.")

	find_library(ZLIB_LIBRARY
		NAMES z libz libz.a ${CMAKE_STATIC_LIBRARY_PREFIX}z${CMAKE_STATIC_LIBRARY_SUFFIX}
		PATHS ${FIND_MSTOOLKIT_PATHS}
		DOC "Path to zlib library")

	find_library(EXPAT_LIBRARY
		NAMES expat libexpat libexpat.a ${CMAKE_STATIC_LIBRARY_PREFIX}expat${CMAKE_STATIC_LIBRARY_SUFFIX}
		PATHS ${FIND_MSTOOLKIT_PATHS}
		DOC "Path to expat library")

