#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Volk::volk" for configuration "Release"
set_property(TARGET Volk::volk APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Volk::volk PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/libvolk.dll.a"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/libvolk.dll"
  )

list(APPEND _cmake_import_check_targets Volk::volk )
list(APPEND _cmake_import_check_files_for_Volk::volk "${_IMPORT_PREFIX}/lib/libvolk.dll.a" "${_IMPORT_PREFIX}/bin/libvolk.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
