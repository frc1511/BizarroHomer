cmake_minimum_required(VERSION 3.1)

project(BizarroHomerAll)

# Initialize directories, files, etc.
install(CODE "execute_process(COMMAND sudo ./scripts/init_workspace.sh)")

#
# Built thirdparty libraries.
#

set(BH_THIRDPARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty")

# fmt
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/fmt")

# Phoenix
add_subdirectory("${BH_THIRDPARTY_DIR}/Phoenix")

#
# Build stuff.
#

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/shared")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/main")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dashboard_server")

#
# Install files.
#

# Install dashboard site.
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/dashboard" DESTINATION "/var/frc1511/BizarroHomer")

# Install music files.
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/music" DESTINATION "/var/frc1511/BizarroHomer")
