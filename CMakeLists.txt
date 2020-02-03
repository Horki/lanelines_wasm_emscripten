cmake_minimum_required(VERSION 3.11.3)

project(LaneLines CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include_directories("/usr/include/opencv4")

# conan_basic_setup()
# include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

find_package(OpenCV REQUIRED)

# Add project executable
add_executable(${PROJECT_NAME} "src/main.cc")
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-s DEMANGLE_SUPPORT=1 -O3 --llvm-lto 1 --closure 1 -s WASM=1 --bind")
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
