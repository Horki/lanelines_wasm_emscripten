cmake_minimum_required(VERSION 3.11.3)

project(LaneLines CXX)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wall -std=c++17 -pthread")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# Add project executable
add_executable(${PROJECT_NAME} "src/main.cc")
target_link_libraries(${PROJECT_NAME} ${CONAN_LIBS})
# target_include_directories(${PROJECT_NAME}
# PUBLIC
# ${PROJECT_SOURCE_DIR}/include)
