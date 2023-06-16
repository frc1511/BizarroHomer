cmake_minimum_required(VERSION 3.1)

project(Phoenix LANGUAGES C CXX) 

add_library(${PROJECT_NAME} INTERFACE)

target_link_libraries(${PROJECT_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/lib/libCTRE_Phoenix.so")
target_link_libraries(${PROJECT_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/lib/libCTRE_PhoenixCCI.so")

target_include_directories(${PROJECT_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/lib/libCTRE_Phoenix.so"
              "${CMAKE_CURRENT_SOURCE_DIR}/lib/libCTRE_PhoenixCCI.so" DESTINATION "/usr/local/lib")
