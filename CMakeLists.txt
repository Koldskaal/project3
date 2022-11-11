# CMakeList.txt : CMake project for portaudio testing, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("portaudio testing")

# Add source to this project's executable.
add_executable (CMakeTarget "main.cpp" "src/waveTable.cpp" "src/DTMF.cpp" "src/transmitter.cpp")

find_package(portaudio REQUIRED)
target_link_libraries(CMakeTarget PRIVATE portaudio)
target_include_directories(CMakeTarget PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# TODO: Add tests and install targets if needed.
