
set(FIND_UTILS_PATHS
        ${PROJECT_SOURCE_DIR}/../utils
        ${PROJECT_SOURCE_DIR}/../build/utils
        ${PROJECT_SOURCE_DIR}/../utils_build
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

