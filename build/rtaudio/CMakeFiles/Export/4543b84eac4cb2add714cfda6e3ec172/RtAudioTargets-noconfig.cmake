#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "RtAudio::rtaudio" for configuration ""
set_property(TARGET RtAudio::rtaudio APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(RtAudio::rtaudio PROPERTIES
  IMPORTED_IMPLIB_NOCONFIG "${_IMPORT_PREFIX}/lib/librtaudio.dll.a"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/bin/librtaudio.dll"
  )

list(APPEND _cmake_import_check_targets RtAudio::rtaudio )
list(APPEND _cmake_import_check_files_for_RtAudio::rtaudio "${_IMPORT_PREFIX}/lib/librtaudio.dll.a" "${_IMPORT_PREFIX}/bin/librtaudio.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
