cmake_minimum_required(VERSION 3.16)

set(CMAKE_TOOLCHAIN_FILE "/usr/local/share/z88dk/cmake/Toolchain-zcc.cmake" CACHE FILEPATH "Toolchain file for cross-compilation")
set(ZCCTARGET test CACHE STRING "Target")

message(STATUS "Using toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "Target: ${ZCCTARGET}")

project(testbot C)
add_executable(testbot bot_api.c test_bot.c)
target_link_options(testbot PUBLIC "-zorg=8192")
