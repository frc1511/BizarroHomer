cmake_minimum_required(VERSION 3.1)

project(BizarroHomerMain)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE BH_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(${PROJECT_NAME} ${BH_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
if(NOT Threads_FOUND)
  message(FATAL_ERROR "Pthread not found.")
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC Threads::Threads)

# SDL2
find_package(SDL2 REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${SDL2_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${SDL2_LIBRARIES})

# fmt
target_link_libraries(${PROJECT_NAME} PUBLIC fmt::fmt)

# Phoenix
target_link_libraries(${PROJECT_NAME} PUBLIC Phoenix)
