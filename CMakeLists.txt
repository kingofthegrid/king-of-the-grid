cmake_minimum_required(VERSION 3.22)

if (APPLE)
    project(engine C CXX OBJC)
else()
    project(engine C CXX)
endif()

set(CMAKE_CXX_STANDARD 20)

set(Z80_WITH_EXECUTE TRUE)
set(Z80_WITH_SPECIAL_RESET TRUE)

add_subdirectory(external/Zeta)
add_subdirectory(external/z80 Z80)

file(GLOB_RECURSE kotg_FILES "src/*.cpp")

add_executable(kotg ${kotg_FILES})

target_link_libraries(kotg PUBLIC Z80)

target_include_directories(kotg PUBLIC "include")
target_include_directories(kotg PUBLIC "external/FastNoise")

set_target_properties(kotg
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
