cmake_minimum_required(VERSION 3.11.3)

project(LaneLines CXX)

set(CMAKE_CXX_STANDARD 11)

# Package is not compiling to emscripten target!!!
# set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
# find_package(OpenCV REQUIRED)

# Conan build for emscripten is not working!!!
# conan_basic_setup()
# include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

include_directories($ENV{HOME}/opencv/include)
include_directories($ENV{HOME}/opencv/build_wasm)
file(GLOB opencv_include_modules "$ENV{HOME}/opencv/modules/*/include")
include_directories(${opencv_include_modules})

file(GLOB opencv_core "$ENV{HOME}/opencv/build_wasm/lib/*.a")

add_executable(${PROJECT_NAME} "src/main.cc")
target_link_libraries(${PROJECT_NAME} ${opencv_core})
set_target_properties(${PROJECT_NAME}
  PROPERTIES LINK_FLAGS
    "-O3 \
    --llvm-lto 1 \
    --closure 1 \
    --bind \
    -s DEMANGLE_SUPPORT=1 \
    -s USE_ZLIB=1 \
    -s WASM=1"
)

