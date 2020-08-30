
set(FIND_UTILS_PATHS
		${CMAKE_BINARY_DIR}/lib
		${CMAKE_BINARY_DIR}/utils/lib
		${CMAKE_CURRENT_BINARY_DIR}/../utils
		${CMAKE_CURRENT_BINARY_DIR}/../build/utils
		${CMAKE_CURRENT_BINARY_DIR}/../utils_build
		${CMAKE_CURRENT_SOURCE_DIR}/../lib
		${CMAKE_CURRENT_SOURCE_DIR}/../utils
		${CMAKE_CURRENT_SOURCE_DIR}/../utils/lib
		${CMAKE_CURRENT_SOURCE_DIR}/lib
		${CMAKE_CURRENT_SOURCE_DIR}/utils
        /usr/local
        /usr/local/lib)

find_path(UTILS_INCLUDE_DIR
            bufferFile.hpp
            fastaFile.hpp
			msScan.hpp
			exceptions.hpp
			ms2.hpp
            molecularFormula.hpp
            peptideUtils.hpp
            tsvFile.hpp
            utils.hpp
        PATH_SUFFIXES include
        PATHS ${FIND_UTILS_PATHS})

find_library(UTILS_LIBRARY
	NAMES utils libutils.a libutils ${CMAKE_STATIC_LIBRARY_PREFIX}utils${CMAKE_STATIC_LIBRARY_SUFFIX}
	PATHS ${FIND_UTILS_PATHS}
	DOC "Path to utils library.")

find_library(ZLIB_LIBRARY
		NAMES z libz libz.a ${CMAKE_STATIC_LIBRARY_PREFIX}z${CMAKE_STATIC_LIBRARY_SUFFIX}
		PATHS ${FIND_MSTOOLKIT_PATHS}
		DOC "Path to zlib library")

