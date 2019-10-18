
set(FIND_UTILS_PATHS
        /usr/local
        ${PROJECT_SOURCE_DIR}/../utils
        ${PROJECT_SOURCE_DIR}/../utils/lib
        ${PROJECT_SOURCE_DIR}/../utils_build
        )

find_path(UTILS_INCLUDE_DIR
            bufferFile.hpp
            fastaFile.hpp
            molecularFormula.hpp
            peptideUtils.hpp
            tsvFile.hpp
            utils.hpp
        PATH_SUFFIXES include
        PATHS ${FIND_UTILS_PATHS})

find_library(UTILS_LIB
        NAMES utils
        HINTS ${FIND_UTILS_PATHS})

