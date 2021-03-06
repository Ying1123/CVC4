###############################################################################
# Top contributors (to current version):
#   Makai Mann, Mathias Preiner, Yoni Zohar
#
# This file is part of the cvc5 project.
#
# Copyright (c) 2009-2021 by the authors listed in the file AUTHORS
# in the top-level source directory and their institutional affiliations.
# All rights reserved.  See the file COPYING in the top-level source
# directory for licensing information.
# #############################################################################
#
# The build system configuration.
##

if(POLICY CMP0057)
  # For cmake >= 3.3 this policy changed the behavior of IN_LIST
  # if the policy exists, we use the NEW behavior
  cmake_policy(SET CMP0057 NEW)
endif()

# Check that scikit-build is installed
# Provides CMake files for Python bindings
check_python_module("skbuild" "scikit-build")

# setuptools for installing the module
check_python_module("setuptools")

# Find cmake modules distributed with scikit-build
# They are distributed under <scikit-build directory>/resources/cmake
execute_process(
  COMMAND
    ${PYTHON_EXECUTABLE} -c "from __future__ import print_function; \
      import os; import skbuild; \
      cmake_module_path=os.path.join(os.path.dirname(skbuild.__file__), \
        'resources', 'cmake'); print(cmake_module_path, end='')"
  OUTPUT_VARIABLE
    SKBUILD_CMAKE_MODULE_PATH
  RESULT_VARIABLE
    RET_SKBUILD_CMAKE_MODULE_PATH
)

if (NOT EXISTS ${SKBUILD_CMAKE_MODULE_PATH})
  message(FATAL_ERROR "Expected CMake module path from
                       scikit-build at ${SKBUILD_CMAKE_MODULE_PATH}")
endif()

# Add scikit-build cmake files to cmake module path
# Required for Cython target below
list(APPEND CMAKE_MODULE_PATH ${SKBUILD_CMAKE_MODULE_PATH})

find_package(PythonExtensions REQUIRED)
find_package(Cython 0.29 REQUIRED)

set(CYTHON_FLAGS "-X embedsignature=True")

# Generate cvc5kinds.{pxd,pyx}
configure_file(genkinds.py.in genkinds.py)
set(GENERATED_FILES
  "${CMAKE_CURRENT_BINARY_DIR}/cvc5kinds.pxd"
  "${CMAKE_CURRENT_BINARY_DIR}/cvc5kinds.pxi"
)
add_custom_command(
  OUTPUT
    ${GENERATED_FILES}
  BYPRODUCTS
    ${GENERATED_FILES}
  COMMAND
    "${PYTHON_EXECUTABLE}"
    "${CMAKE_CURRENT_BINARY_DIR}/genkinds.py"
    --kinds-header "${PROJECT_SOURCE_DIR}/src/api/cpp/cvc5_kind.h"
    --kinds-file-prefix "${CMAKE_CURRENT_BINARY_DIR}/cvc5kinds"
  DEPENDS
    "${CMAKE_CURRENT_BINARY_DIR}/genkinds.py"
    "${PROJECT_SOURCE_DIR}/src/api/cpp/cvc5_kind.h"
)

add_custom_target(cvc5kinds DEPENDS ${GENERATED_FILES})

include_directories(${CMAKE_CURRENT_BINARY_DIR})   # for cvc5kinds.pxi
add_cython_target(pycvc5 CXX)

add_library(pycvc5
            MODULE
            ${pycvc5})
add_dependencies(pycvc5 cvc5kinds)

target_include_directories(pycvc5
  PRIVATE
  ${PYTHON_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/src       # for API headers in src/api/cpp
  ${CMAKE_BINARY_DIR}/src         # for cvc5_export.h
)

target_link_libraries(pycvc5 cvc5 ${PYTHON_LIBRARIES})

# Disable -Werror and other warnings for code generated by Cython.
# Note: Visibility is reset to default here since otherwise the PyInit_...
# function will not be exported correctly
# (https://github.com/cython/cython/issues/3380).
set_target_properties(pycvc5
                      PROPERTIES
                      COMPILE_FLAGS
                        "-Wno-error -Wno-shadow -Wno-implicit-fallthrough"
                      CXX_VISIBILITY_PRESET default
                      VISIBILITY_INLINES_HIDDEN 0
                      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/pycvc5")

python_extension_module(pycvc5)

# Installation based on https://bloerg.net/2012/11/10/cmake-and-distutils.html
# Create a wrapper python directory and generate a distutils setup.py file
configure_file(setup.py.in setup.py)
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/pycvc5/__init__.py
"import sys
from .pycvc5 import *
# fake a submodule for dotted imports, e.g. from pycvc5.kinds import *
sys.modules['%s.%s'%(__name__, kinds.__name__)] = kinds")

# figure out if we're in a virtualenv
execute_process(OUTPUT_VARIABLE IN_VIRTUALENV
  COMMAND
  "${PYTHON_EXECUTABLE}"
  -c
  "from __future__ import print_function; import os;
print('YES' if 'VIRTUAL_ENV' in os.environ else 'NO', end='')")

set(INSTALL_CMD
    "${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/setup.py install")
# if we're in a virtualenv, we install it in the virtualenv lib location
# otherwise install in site-packages
# option --single-version-externally-managed prevents it from being
# an .egg distribution and --record records the list of installed files
if ("${IN_VIRTUALENV}" STREQUAL "NO")
  set(INSTALL_CMD "${INSTALL_CMD}
          --prefix=${CMAKE_INSTALL_PREFIX}
          --single-version-externally-managed
          --record=pycvc5-installed-files.txt")
endif()

message("Python bindings install command: ${INSTALL_CMD}")

install(CODE "execute_process(COMMAND ${INSTALL_CMD})"
        FILE_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ)
