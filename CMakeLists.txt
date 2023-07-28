cmake_minimum_required(VERSION 3.1)

project(BizarroHomerAll)

if(WIN32)
  message(FATAL_ERROR "Windows builds are not allowed")
endif()

# Initialize directories, files, etc.
install(CODE "execute_process(COMMAND sudo ./scripts/init_workspace.sh)")

# --- Built thirdparty libraries ---

set(BH_THIRDPARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty")

# fmt
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/fmt")

# Phoenix
add_subdirectory("${BH_THIRDPARTY_DIR}/Phoenix")

# --- Build programs ---

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/shared")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/main")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dashboard_server")

# Install dashboard site.
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/dashboard" DESTINATION "/var/frc1511/BizarroHomer")
