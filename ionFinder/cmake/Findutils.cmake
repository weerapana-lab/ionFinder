
set(FIND_UTILS_PATHS
        ${PROJECT_SOURCE_DIR}/../utils
        ${PROJECT_SOURCE_DIR}/../build/utils
        ${PROJECT_SOURCE_DIR}/../utils_build
        ${PROJECT_SOURCE_DIR}/../lib
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

find_library(UTILS_LIB
        NAMES utils
        HINTS ${FIND_UTILS_PATHS})

