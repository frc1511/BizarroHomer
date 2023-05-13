cmake_minimum_required(VERSION 3.1)

project(BizarroHomer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE BH_SOURCES "src/*.cpp")

add_executable(${PROJECT_NAME} ${BH_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")

set(BH_THIRDPARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty")

add_subdirectory("${BH_THIRDPARTY_DIR}/fmt")

target_link_libraries(${PROJECT_NAME} PUBLIC fmt::fmt)

add_subdirectory("${BH_THIRDPARTY_DIR}/Phoenix")

target_link_libraries(${PROJECT_NAME} PUBLIC Phoenix)
