cmake_minimum_required(VERSION 3.1)

project(BizarroHomerShared)

set(IPC_PATHNAME "/var/frc1511/BizarroHomer/ipc_msg_queue_key" CACHE BOOL "IPC Message Queue Key Path" FORCE)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE BH_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_library(${PROJECT_NAME} ${BH_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")

# fmt
target_link_libraries(${PROJECT_NAME} PUBLIC fmt::fmt)

# Config file
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/include/BizarroHomerShared/Config/Config.hpp.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/BizarroHomerShared/Config/Config.hpp"
)
