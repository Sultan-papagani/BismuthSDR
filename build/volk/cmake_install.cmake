# Install script for directory: C:/Users/sultan/Desktop/BismuthSDR/volk

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/BismuthSDR")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "C:/mingw64/bin/objdump.exe")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "volk_devel" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "C:/Users/sultan/Desktop/BismuthSDR/build/volk/volk.pc")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "volk_devel" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE DIRECTORY FILES "C:/Users/sultan/Desktop/BismuthSDR/volk/kernels/volk" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "volk_devel" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/volk" TYPE FILE FILES
    "C:/Users/sultan/Desktop/BismuthSDR/volk/include/volk/volk_prefs.h"
    "C:/Users/sultan/Desktop/BismuthSDR/volk/include/volk/volk_alloc.hh"
    "C:/Users/sultan/Desktop/BismuthSDR/volk/include/volk/volk_complex.h"
    "C:/Users/sultan/Desktop/BismuthSDR/volk/include/volk/volk_common.h"
    "C:/Users/sultan/Desktop/BismuthSDR/volk/include/volk/saturation_arithmetic.h"
    "C:/Users/sultan/Desktop/BismuthSDR/volk/include/volk/volk_avx_intrinsics.h"
    "C:/Users/sultan/Desktop/BismuthSDR/volk/include/volk/volk_avx2_intrinsics.h"
    "C:/Users/sultan/Desktop/BismuthSDR/volk/include/volk/volk_avx2_fma_intrinsics.h"
    "C:/Users/sultan/Desktop/BismuthSDR/volk/include/volk/volk_sse_intrinsics.h"
    "C:/Users/sultan/Desktop/BismuthSDR/volk/include/volk/volk_sse3_intrinsics.h"
    "C:/Users/sultan/Desktop/BismuthSDR/volk/include/volk/volk_neon_intrinsics.h"
    "C:/Users/sultan/Desktop/BismuthSDR/build/volk/include/volk/volk.h"
    "C:/Users/sultan/Desktop/BismuthSDR/build/volk/include/volk/volk_cpu.h"
    "C:/Users/sultan/Desktop/BismuthSDR/build/volk/include/volk/volk_config_fixed.h"
    "C:/Users/sultan/Desktop/BismuthSDR/build/volk/include/volk/volk_typedefs.h"
    "C:/Users/sultan/Desktop/BismuthSDR/volk/include/volk/volk_malloc.h"
    "C:/Users/sultan/Desktop/BismuthSDR/build/volk/include/volk/volk_version.h"
    "C:/Users/sultan/Desktop/BismuthSDR/volk/include/volk/constants.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "volk_devel" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/volk" TYPE FILE FILES
    "C:/Users/sultan/Desktop/BismuthSDR/build/volk/cmake/Modules/VolkConfig.cmake"
    "C:/Users/sultan/Desktop/BismuthSDR/build/volk/cmake/Modules/VolkConfigVersion.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/volk/VolkTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/volk/VolkTargets.cmake"
         "C:/Users/sultan/Desktop/BismuthSDR/build/volk/CMakeFiles/Export/286ce4a61b22e286f7f49dc7a82536c7/VolkTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/volk/VolkTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/volk/VolkTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/volk" TYPE FILE FILES "C:/Users/sultan/Desktop/BismuthSDR/build/volk/CMakeFiles/Export/286ce4a61b22e286f7f49dc7a82536c7/VolkTargets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/volk" TYPE FILE FILES "C:/Users/sultan/Desktop/BismuthSDR/build/volk/CMakeFiles/Export/286ce4a61b22e286f7f49dc7a82536c7/VolkTargets-release.cmake")
  endif()
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Users/sultan/Desktop/BismuthSDR/build/volk/cpu_features/cmake_install.cmake")
  include("C:/Users/sultan/Desktop/BismuthSDR/build/volk/docs/cmake_install.cmake")
  include("C:/Users/sultan/Desktop/BismuthSDR/build/volk/lib/cmake_install.cmake")
  include("C:/Users/sultan/Desktop/BismuthSDR/build/volk/tests/cmake_install.cmake")
  include("C:/Users/sultan/Desktop/BismuthSDR/build/volk/apps/cmake_install.cmake")
  include("C:/Users/sultan/Desktop/BismuthSDR/build/volk/python/volk_modtool/cmake_install.cmake")

endif()

