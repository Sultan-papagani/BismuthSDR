# Install script for directory: C:/Users/sultan/Desktop/BismuthSDR/volk/python/volk_modtool

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

if(CMAKE_INSTALL_COMPONENT STREQUAL "volk" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Lib/site-packages/volk_modtool" TYPE FILE FILES
    "C:/Users/sultan/Desktop/BismuthSDR/volk/python/volk_modtool/__init__.py"
    "C:/Users/sultan/Desktop/BismuthSDR/volk/python/volk_modtool/cfg.py"
    "C:/Users/sultan/Desktop/BismuthSDR/volk/python/volk_modtool/volk_modtool_generate.py"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "volk" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Lib/site-packages/volk_modtool" TYPE FILE FILES
    "C:/Users/sultan/Desktop/BismuthSDR/build/volk/python/volk_modtool/__init__.pyc"
    "C:/Users/sultan/Desktop/BismuthSDR/build/volk/python/volk_modtool/cfg.pyc"
    "C:/Users/sultan/Desktop/BismuthSDR/build/volk/python/volk_modtool/volk_modtool_generate.pyc"
    "C:/Users/sultan/Desktop/BismuthSDR/build/volk/python/volk_modtool/__init__.pyo"
    "C:/Users/sultan/Desktop/BismuthSDR/build/volk/python/volk_modtool/cfg.pyo"
    "C:/Users/sultan/Desktop/BismuthSDR/build/volk/python/volk_modtool/volk_modtool_generate.pyo"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "volk" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE PROGRAM RENAME "volk_modtool.py" FILES "C:/Users/sultan/Desktop/BismuthSDR/build/volk/python/volk_modtool/volk_modtool.exe")
endif()

