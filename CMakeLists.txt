cmake_minimum_required(VERSION 3.11.3)

project(LaneLines CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(OPENCV_PATH opencv)

# Most Warnings for clang
add_compile_options(-Wall -Wextra -pedantic -Werror)

if (${CMAKE_BUILD_TYPE} MATCHES Debug)
  message("set debug properties with sanitarzes")
  set(target_properties "-fsanitize=address  -fsanitize=leak -g")
endif ()

if (${CMAKE_BUILD_TYPE} MATCHES Release)
  message("set release properties")
  set(target_properties "-03 --closure 1")
endif ()

include_directories(${OPENCV_PATH}/include)
include_directories(${OPENCV_PATH}/build_wasm)
file(GLOB opencv_include_modules "${OPENCV_PATH}/modules/*/include")
include_directories(${opencv_include_modules})

file(GLOB opencv_core "${OPENCV_PATH}/build_wasm/lib/*.a")
file(GLOB opencv_lib_core "${OPENCV_PATH}/build_wasm/lib/libopencv_core.a")

file(GLOB source_files src/*.cc)

add_executable(${PROJECT_NAME} ${source_files})
target_link_libraries(${PROJECT_NAME} ${opencv_core} ${opencv_lib_core})
target_include_directories(${PROJECT_NAME}
   PUBLIC
   ${PROJECT_SOURCE_DIR}/include)
set_target_properties(${PROJECT_NAME}
  PROPERTIES LINK_FLAGS
    "${target_properties} \
    --bind \
    -s DEMANGLE_SUPPORT=1 \
    -s USE_ZLIB=1 \
    -s WASM=1"
)

# For Later use!
#-s ALLOW_MEMORY_GROWTH=1 \
#-s WASM_OBJECT_FILES=0 \
