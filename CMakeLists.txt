cmake_minimum_required(VERSION 3.22)

if (APPLE)
    project(engine C CXX OBJC)
else()
    project(engine C CXX)
endif()

set(CMAKE_CXX_STANDARD 20)

set(Z80_WITH_EXECUTE TRUE)
set(Z80_WITH_SPECIAL_RESET TRUE)

add_subdirectory(external/SDL SDL)
add_subdirectory(external/Zeta)
add_subdirectory(external/z80 Z80)
add_subdirectory(external/SDL_image SDL_ttf)

set(SDL_SHARED OFF) # Disable shared library
set(SDL_STATIC ON)  # Enable static library
set(SDL_IMAGE_PNG ON)   # Enable PNG support
set(SDL_IMAGE_JPG OFF)  # Disable JPEG support
set(SDL_IMAGE_TIF OFF)  # Disable TIFF support

file(GLOB_RECURSE kotg_FILES "src/*.cpp")
file(GLOB_RECURSE SDL2_FILES "sdl/*.cpp")

add_executable(kotg ${kotg_FILES})
add_executable(kotg_sdl ${kotg_FILES} ${SDL2_FILES})

target_link_libraries(kotg PUBLIC Z80)
target_link_libraries(kotg_sdl PUBLIC Z80)

target_include_directories(kotg PUBLIC "include")
target_include_directories(kotg_sdl PUBLIC "include")
target_include_directories(kotg_sdl PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL/include")
target_include_directories(kotg_sdl PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL_image/include")
target_compile_definitions(kotg_sdl PUBLIC -D__SDL)

target_include_directories(kotg_sdl PUBLIC
    ${SDL2_INCLUDE_DIRS})

target_link_libraries(kotg_sdl PRIVATE SDL3::SDL3 SDL3_image::SDL3_image)

set_target_properties(kotg
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

set_target_properties(kotg_sdl
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
