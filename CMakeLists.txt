cmake_minimum_required(VERSION 3.29)
project(SDR)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SRC "src/*.cpp" "src/*.c")
file(GLOB_RECURSE SRC_IMGUI "imgui/*.cpp" "imgui/*.c")
file(GLOB_RECURSE SRC_RTLSDR "rtlsdr/*.cpp" "rtlsdr/*.c")

add_executable(SDR main.cpp ${SRC_IMGUI} ${SRC} ${SRC_RTLSDR})

target_include_directories(SDR PUBLIC ${CMAKE_CURRENT_LIST_DIR}/GLFW/)
target_include_directories(SDR PUBLIC ${CMAKE_CURRENT_LIST_DIR}/imgui/)
target_include_directories(SDR PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src/)
target_include_directories(SDR PUBLIC ${CMAKE_CURRENT_LIST_DIR}/rtlsdr/)
target_include_directories(SDR PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src/dsp)

add_subdirectory("volk/")
target_link_libraries(SDR PUBLIC volk)
target_link_libraries(SDR PUBLIC ${CMAKE_CURRENT_LIST_DIR}/rtlsdr/librtlsdr.dll)


#glfw3
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(SDR PUBLIC glfw)

#opengl
find_package(OpenGL REQUIRED)
target_link_libraries(SDR PUBLIC OpenGL::GL)

#fftw3
find_package(FFTW3f CONFIG REQUIRED)
target_link_libraries(SDR PUBLIC FFTW3::fftw3f)

