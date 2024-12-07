cmake_minimum_required(VERSION 3.22)

if(EMSCRIPTEN)
    message(STATUS "Building with Emscripten")

    project(engine C CXX)

    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -sWASM=1 -SNO_DISABLE_EXCEPTION_CATCHING=1 -sINITIAL_HEAP=65536000 -sALLOW_MEMORY_GROWTH=1 -sEXPORTED_FUNCTIONS=['ccall','cwrap','_malloc','_free'] -sEXPORT_NAME='KOTG'")

else()
    if (APPLE)
        project(engine C CXX OBJC)
    else()
        project(engine C CXX)
    endif()
endif()

set(CMAKE_CXX_STANDARD 20)

set(Z80_WITH_EXECUTE TRUE)
set(Z80_WITH_SPECIAL_RESET TRUE)

set(Zeta_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/Zeta/API")
set(Zeta_VERSION "1.0")

add_subdirectory(external/z80 Z80)

file(GLOB_RECURSE kotg_FILES "src/*.cpp")

add_executable(kotg ${kotg_FILES})

target_link_libraries(kotg PUBLIC Z80)

target_include_directories(kotg PUBLIC "include")
target_include_directories(kotg PUBLIC "external/FastNoise")

set_target_properties(kotg
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

if(EMSCRIPTEN)
    add_custom_command(TARGET kotg POST_BUILD
        COMMAND cp -r "${CMAKE_CURRENT_SOURCE_DIR}/web/*" "${CMAKE_CURRENT_SOURCE_DIR}/bin"
        COMMENT "Copy web resources"
    )
endif()