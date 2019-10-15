
set(FIND_UTILS_PATHS
        /usr/local
        ${PROJECT_SOURCE_DIR}/../utils
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

find_path(UTILS_LIB
        NAMES utils
        PATH_SUFFIXES lib
        PATHS ${FIND_UTILS_PATHS})

