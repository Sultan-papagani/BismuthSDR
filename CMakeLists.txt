cmake_minimum_required(VERSION 3.29)
project(BismuthSDR)

set(CMAKE_CXX_STANDARD 20)

set(CXXFLAGS  "-g")
set(CMAKE_CXX_FLAGS "${CXXFLAGS}")

file(GLOB_RECURSE SRC "src/*.cpp" "src/*.c")
file(GLOB_RECURSE SRC_IMGUI "imgui/*.cpp" "imgui/*.c")

add_executable(BismuthSDR main.cpp ${SRC_IMGUI} ${SRC} ${SRC_RTLSDR} "graphics/bismuthsdr_icon.o")

target_include_directories(BismuthSDR PUBLIC ${CMAKE_CURRENT_LIST_DIR}/GLFW/)
target_include_directories(BismuthSDR PUBLIC ${CMAKE_CURRENT_LIST_DIR}/imgui/)
target_include_directories(BismuthSDR PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src/)

#rtl-sdr source
target_link_libraries(BismuthSDR PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src/rtlsdr/librtlsdr.dll)

#volk
add_subdirectory("volk/")
target_link_libraries(BismuthSDR PUBLIC volk)

#rtaudio
add_subdirectory("rtaudio/")
target_link_libraries(BismuthSDR PUBLIC rtaudio)

#glfw3
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(BismuthSDR PUBLIC glfw)

#opengl
find_package(OpenGL REQUIRED)
target_link_libraries(BismuthSDR PUBLIC OpenGL::GL)

#fftw3
find_package(FFTW3f CONFIG REQUIRED)
target_link_libraries(BismuthSDR PUBLIC FFTW3::fftw3f)

