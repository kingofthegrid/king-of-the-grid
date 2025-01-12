cmake_minimum_required(VERSION 3.16)

set(CMAKE_TOOLCHAIN_FILE "/usr/local/share/z88dk/cmake/Toolchain-zcc.cmake" CACHE FILEPATH "Toolchain file for cross-compilation")
set(ZCCTARGET test CACHE STRING "Target")

message(STATUS "Using toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "Target: ${ZCCTARGET}")

project(test-bot-z88dk C ASM)
add_executable(test-bot-z88dk ../../bot-api/z88dk-backend.asm test_bot.c)
target_include_directories(test-bot-z88dk PUBLIC "../../bot-api")
target_link_options(test-bot-z88dk PUBLIC "-pragma-define=REGISTER_SP=57344")
