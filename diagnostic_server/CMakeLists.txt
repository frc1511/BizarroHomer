cmake_minimum_required(VERSION 3.1)

project(BizarroHomerDiagnosticServer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE BH_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(${PROJECT_NAME} ${BH_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/")

target_link_libraries(${PROJECT_NAME} PRIVATE BizarroHomerShared)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
if(NOT Threads_FOUND)
  message(FATAL_ERROR "Pthread not found.")
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

# fmt
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)

# Install program.
install(TARGETS ${PROJECT_NAME} DESTINATION "/usr/local/bin")

# Install systemd service.
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/BizarroHomerDiagnosticServer.service" DESTINATION "/etc/systemd/system")

# Enable systemd service.
install(CODE "execute_process(COMMAND systemctl enable BizarroHomerDiagnosticServer.service)")
