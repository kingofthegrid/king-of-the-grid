cmake_minimum_required(VERSION 3.27)
project(engine)

set(CMAKE_CXX_STANDARD 20)

find_package(SDL2)
find_package(SDL2_ttf)
find_package(SDL2_image)

set(Z80_WITH_EXECUTE TRUE)
set(Z80_WITH_SPECIAL_RESET TRUE)

add_subdirectory(external/Zeta)
add_subdirectory(external/Z80 Z80)

file(GLOB_RECURSE kotg_FILES "src/*.cpp")
file(GLOB_RECURSE SDL2_FILES "sdl2/*.cpp")

add_executable(kotg ${kotg_FILES})
add_executable(kotg_sdl2 ${kotg_FILES} ${SDL2_FILES})

target_link_libraries(kotg PUBLIC Z80)
target_link_libraries(kotg_sdl2 PUBLIC Z80)

target_include_directories(kotg PUBLIC "include")
target_include_directories(kotg_sdl2 PUBLIC "include")
target_compile_definitions(kotg_sdl2 PUBLIC -D__SDL2)

target_include_directories(kotg_sdl2 PUBLIC
    ${SDL2_INCLUDE_DIRS})

if(WIN32)
    target_link_libraries(kotg_sdl2 PUBLIC SDL2main SDL2 SDL2_ttf)
else()
    target_link_libraries(kotg_sdl2 PUBLIC ${SDL2_LIBRARIES} SDL2_ttf::SDL2_ttf SDL2_image::SDL2_image)
endif()

set_target_properties(kotg
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

set_target_properties(kotg_sdl2
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
