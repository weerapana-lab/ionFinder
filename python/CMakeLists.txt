
option(COPY_PYTHON_TO_BUILD "Copy python files to build directory?" ON)
find_program(BASH bash DOC "Path to bash interpreter.")

# Add required python modules here for now
# find_python_module(matplotlib REQUIRED)
find_python_module(pandas REQUIRED)
find_program(RSCRIPT Rscript Doc "Path to Rscript command.")
set(SRC_SCRIPTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../scripts)
set(SRC_ETC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../etc)

if(COPY_PYTHON_TO_BUILD MATCHES ON)
	macro(copy_files GLOBPAT DESTINATION TARGET_NAME)
		file(GLOB_RECURSE COPY_FILES
				RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
				${GLOBPAT})
		# auto generated files
		list(FILTER COPY_FILES EXCLUDE REGEX "_constants\.py$")
		add_custom_target(${TARGET_NAME} ALL
				COMMENT "Copying files: ${GLOBPAT}")

		foreach(FILENAME ${COPY_FILES})
			set(SRC "${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME}")
			set(DST "${DESTINATION}/${FILENAME}")

			add_custom_command(
					TARGET ${TARGET_NAME}
					COMMAND ${CMAKE_COMMAND} -E copy ${SRC} ${DST}
			)
		endforeach(FILENAME)
	endmacro(copy_files)

	set(PYTHON_SRC_DIR ${CMAKE_CURRENT_BINARY_DIR}/python)
	file(MAKE_DIRECTORY ${PYTHON_SRC_DIR}/modules)
	file(MAKE_DIRECTORY ${PYTHON_SRC_DIR}/parsers/modules)
	copy_files(${CMAKE_CURRENT_SOURCE_DIR}/../python/*.py ${PYTHON_SRC_DIR} python_copy)
	add_configure_file(spectrum_constants SpectrumConstantsPy "python/modules" "py")
	add_configure_file(tsv_constants TSVConstantsPy "python/parsers/modules" "py")
	add_configure_file(scaffold_constants ScaffoldConstantsPy "python/parsers/modules" "py")
	add_configure_file(maxquant_constants MaxQuantConstantsPy "python/parsers/modules" "py")
else()
	set(PYTHON_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../python)
endif()

if(NOT DEFINED PYTHON_RUNTIME_OUTPUT_DIR)
	set(PYTHON_RUNTIME_OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin)
endif()
set(RUN_MAKE_MS2_SCR_PATH ${PYTHON_SRC_DIR}/run_make_ms2.py
		CACHE STRING "Path to run_make_ms2.py")
set(MAKE_MS2_SRC_PATH ${PYTHON_SRC_DIR}/make_ms2.py
		CACHE STRING "Path to make_ms2.py")
set(PARSE_SCAFFOLD_PATH ${PYTHON_SRC_DIR}/parsers/parse_scaffold.py
		CACHE STRING "Path to parse_scaffold.py")
set(PARSE_MAXQUANT_PATH ${PYTHON_SRC_DIR}/parsers/parse_maxquant.py
		CACHE STRING "Path to parse_scaffold.py")
set(PYTHON_BUILD_SCRIPT ${SRC_SCRIPTS_DIR}/addPythonFiles.sh
		CACHE STRING "Path to addPythonFiles.sh")

string(CONCAT MAKE_MS2_MAIN_ARGS
	   ${CMAKE_CURRENT_SOURCE_DIR}/../rscripts/makeMs2.R
       "', '" ${RSCRIPT})
add_custom_command(OUTPUT ${PYTHON_RUNTIME_OUTPUT_DIR}/make_ms2
		DEPENDS $<$<BOOL:${COPY_PYTHON_TO_BUILD}>:python_copy> ${MAKE_MS2_SRC_PATH}
		COMMENT "Adding make_ms2 executable."
		VERBATIM
		COMMAND
			${BASH} ${PYTHON_BUILD_SCRIPT}
			--pythonExe ${PYTHON_EXE}
			--binDir ${PYTHON_RUNTIME_OUTPUT_DIR}
			--mainArgs ${MAKE_MS2_MAIN_ARGS}
			${MAKE_MS2_SRC_PATH}
		)
add_custom_target(make_ms2 ALL
	              DEPENDS ${PYTHON_RUNTIME_OUTPUT_DIR}/make_ms2)

	add_custom_command(OUTPUT ${PYTHON_RUNTIME_OUTPUT_DIR}/run_make_ms2
		DEPENDS $<$<BOOL:${COPY_PYTHON_TO_BUILD}>:python_copy> ${RUN_MAKE_MS2_SCR_PATH}
		COMMENT "Adding run_make_ms2 executable."
		COMMAND
			${BASH} ${PYTHON_BUILD_SCRIPT}
			--pythonExe ${PYTHON_EXE}
			--binDir ${PYTHON_RUNTIME_OUTPUT_DIR}
			${RUN_MAKE_MS2_SCR_PATH}
		)
add_custom_target(run_make_ms2 ALL
	              DEPENDS ${PYTHON_RUNTIME_OUTPUT_DIR}/run_make_ms2)

add_custom_command(
	OUTPUT ${PYTHON_RUNTIME_OUTPUT_DIR}/parse_scaffold
		DEPENDS $<$<BOOL:${COPY_PYTHON_TO_BUILD}>:python_copy> ${PARSE_SCAFFOLD_PATH}
		COMMENT "Adding parse_scaffold executable."
		COMMAND
			${BASH} ${PYTHON_BUILD_SCRIPT}
			--pythonExe ${PYTHON_EXE}
			--binDir ${PYTHON_RUNTIME_OUTPUT_DIR}
			${PARSE_SCAFFOLD_PATH}
		)
add_custom_target(parse_scaffold ALL
                  DEPENDS ${PYTHON_RUNTIME_OUTPUT_DIR}/parse_scaffold)

add_custom_command(
	OUTPUT ${PYTHON_RUNTIME_OUTPUT_DIR}/parse_maxquant
		DEPENDS $<$<BOOL:${COPY_PYTHON_TO_BUILD}>:python_copy> ${PARSE_MAXQUANT_PATH}
		COMMENT "Adding parse_maxquant executable."
		COMMAND
			${BASH} ${PYTHON_BUILD_SCRIPT}
			--pythonExe ${PYTHON_EXE}
			--binDir ${PYTHON_RUNTIME_OUTPUT_DIR}
			${PARSE_MAXQUANT_PATH}
		)
add_custom_target(parse_maxquant ALL
	              DEPENDS ${PYTHON_RUNTIME_OUTPUT_DIR}/parse_maxquant)

install(CODE "execute_process(COMMAND ${BASH} ${PYTHON_BUILD_SCRIPT}
                                --pythonExe ${PYTHON_EXE}
                                --binDir ${CMAKE_INSTALL_PREFIX}/bin
                                ${RUN_MAKE_MS2_SCR_PATH}
                              COMMAND_ECHO NONE)"
)
install(CODE "execute_process(COMMAND ${BASH} ${PYTHON_BUILD_SCRIPT}
			                    --pythonExe ${PYTHON_EXE}
                                --binDir ${CMAKE_INSTALL_PREFIX}/bin
                                --mainArgs \"${MAKE_MS2_MAIN_ARGS}\"
                                ${MAKE_MS2_SRC_PATH})"
)
install(CODE "execute_process(COMMAND ${BASH} ${PYTHON_BUILD_SCRIPT}
 			                    --pythonExe ${PYTHON_EXE}
                                --binDir ${CMAKE_INSTALL_PREFIX}/bin
                                ${PARSE_MAXQUANT_PATH})"
)
install(CODE "execute_process(COMMAND ${BASH} ${PYTHON_BUILD_SCRIPT}
 			                    --pythonExe ${PYTHON_EXE}
                                --binDir ${CMAKE_INSTALL_PREFIX}/bin
                                ${PARSE_SCAFFOLD_PATH})"
)
 
