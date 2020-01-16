# Finds a local version of Python
# The original version of this moduele was written by Andre Anjos

include(FindPackageHandleStandardArgs)

find_program(PYTHON_EXECUTABLE python DOC "Path to python interpreter.")
find_package_handle_standard_args(PYTHON_EXECUTABLE DEFAULT_MSG PYTHON_EXECUTABLE)

execute_process(COMMAND ${PYTHON_EXECUTABLE} -c "import sys; print('%d.%d' % (sys.version_info[0], sys.version_info[1]))" OUTPUT_VARIABLE PYTHON_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
set(PYTHON_VERSION "${PYTHON_VERSION}" CACHE STRING "Python version")

# This function checks for python packages that should be installed before you
# try to compile this project
function(find_python_module module)
    string(TOUPPER ${module} module_upper)
  if(NOT PYTHON_${module_upper})
        if(ARGC GREATER 1 AND ARGV1 STREQUAL "REQUIRED")
      set(PYTHON_${module}_FIND_REQUIRED TRUE)
        endif()
        # A module's location is usually a directory, but for binary modules
        # it's a .so file.
    execute_process(COMMAND "${PYTHON_EXECUTABLE}" "-c" 
            "import re, ${module}; print(re.compile('/__init__.py.*').sub('',${module}.__file__))"
            RESULT_VARIABLE _${module}_status 
            OUTPUT_VARIABLE _${module}_location
            ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
        if(NOT _${module}_status)
      set(PYTHON_${module_upper} ${_${module}_location} CACHE STRING 
                "Location of Python module ${module}")
        endif(NOT _${module}_status)
  endif(NOT PYTHON_${module_upper})
  find_package_handle_standard_args(PYTHON_${module} DEFAULT_MSG PYTHON_${module_upper})
endfunction(find_python_module)

