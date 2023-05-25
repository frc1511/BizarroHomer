cmake_minimum_required(VERSION 3.1)

project(BizarroHomerAll)

# fmt
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/fmt")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/main")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dualshock4_led_control")
