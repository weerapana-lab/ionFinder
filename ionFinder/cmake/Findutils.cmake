
set(FIND_PEPTIDE_UTILS_PATHS
		${CMAKE_BINARY_DIR}/lib
		${CMAKE_BINARY_DIR}/peptideUtils/lib
		${CMAKE_CURRENT_BINARY_DIR}/../peptideUtils
		${CMAKE_CURRENT_BINARY_DIR}/../build/peptideUtils
		${CMAKE_CURRENT_BINARY_DIR}/../peptideUtils_build
		${CMAKE_CURRENT_SOURCE_DIR}/../lib
		${CMAKE_CURRENT_SOURCE_DIR}/../peptideUtils
		${CMAKE_CURRENT_SOURCE_DIR}/../peptideUtils/lib
		${CMAKE_CURRENT_SOURCE_DIR}/lib
		${CMAKE_CURRENT_SOURCE_DIR}/peptideUtils
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
            peptideUtils.hpp
        PATH_SUFFIXES include
		PATHS ${FIND_PEPTIDE_UTILS_PATHS})

	find_library(PEPTIDE_UTILS_LIBRARY
	NAMES peptideUtils libpeptideUtils.a libpeptideUtils ${CMAKE_STATIC_LIBRARY_PREFIX}peptideUtils${CMAKE_STATIC_LIBRARY_SUFFIX}
	PATHS ${FIND_PEPTIDE_UTILS_PATHS}
	DOC "Path to peptideUtils library.")

find_library(ZLIB_LIBRARY
		NAMES z libz libz.a ${CMAKE_STATIC_LIBRARY_PREFIX}z${CMAKE_STATIC_LIBRARY_SUFFIX}
		PATHS ${FIND_MSTOOLKIT_PATHS}
		DOC "Path to zlib library")

