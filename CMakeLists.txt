cmake_minimum_required(VERSION 3.1)

project(BizarroHomerAll)

# --- Built thirdparty libraries ---

set(BH_THIRDPARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty")

# fmt
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/fmt")

# Phoenix
add_subdirectory("${BH_THIRDPARTY_DIR}/Phoenix")

# --- Build programs ---

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/shared")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/main")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/diagnostic_server")
