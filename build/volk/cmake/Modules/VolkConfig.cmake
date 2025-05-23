# Copyright 2016, 2018 - 2020 Free Software Foundation, Inc.
#
# This file is part of VOLK.
#
# SPDX-License-Identifier: LGPL-3.0-or-later
#

get_filename_component(VOLK_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

if(NOT TARGET Volk::volk)
    include("${VOLK_CMAKE_DIR}/VolkTargets.cmake")
endif()

# set VOLK_FOUND to be set globally, for whether a compatible Volk was
# found -- could be a correct enough version or any version depending
# on how find_package was called.
if(NOT TARGET Volk::volk)
    set(VOLK_FOUND FALSE)
else()
    set(VOLK_FOUND TRUE)
endif()

# cache whether a compatible Volk was found for
# use anywhere in the calling project
set(VOLK_FOUND
    ${VOLK_FOUND}
    CACHE BOOL "Whether a compatible Volk was found" FORCE)

if(VOLK_FOUND)
    # use the new target library, regardless of whether new or old style
    # we still need to set a variable with the library name so that there
    # is a variable to reference in the using-project's cmake scripts!
    set(VOLK_LIBRARIES
        Volk::volk
        CACHE STRING "Volk Library" FORCE)

    # INTERFACE_INCLUDE_DIRECTORIES should always be set
    get_target_property(VOLK_INCLUDE_DIRS Volk::volk INTERFACE_INCLUDE_DIRECTORIES)
    set(VOLK_INCLUDE_DIRS
        ${VOLK_INCLUDE_DIRS}
        CACHE STRING "Volk Include Directories" FORCE)

    # for backward compatibility with old-CMake non-target project finding
    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(Volk DEFAULT_MSG VOLK_LIBRARIES VOLK_INCLUDE_DIRS)
    mark_as_advanced(VOLK_LIBRARIES VOLK_INCLUDE_DIRS)
endif(VOLK_FOUND)
