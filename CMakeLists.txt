cmake_minimum_required(VERSION 3.16)

set(CMAKE_OSX_SYSROOT "")

set(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR aarch64)
set(CMAKE_CROSSCOMPILING TRUE)

unset(_CMAKE_APPLE_ARCHS_DEFAULT)

set(CMAKE_C_COMPILER_FORCED   1)
set(CMAKE_CXX_COMPILER_FORCED 1)

set(CMAKE_C_COMPILER "/opt/homebrew/bin/aarch64-none-elf-gcc")
set(CMAKE_CXX_COMPILER "/opt/homebrew/bin/aarch64-none-elf-g++")
set(CMAKE_AR "/opt/homebrew/bin/aarch64-none-elf-ar")
set(CMAKE_LINKER "/opt/homebrew/bin/aarch64-none-elf-ld")
set(CMAKE_OBJCOPY "/opt/homebrew/bin/aarch64-none-elf-objcopy")

set(CMAKE_C_FLAGS "")
set(CMAKE_CXX_FLAGS "")
set(CMAKE_CXX_LINK_FLAGS "")

project(pi-writer C CXX)

set(RASPPI 4)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set(CMAKE_COLOR_DIAGNOSTICS ON)

include_directories(/Applications/ArmGNUToolchain/13.2.Rel1/aarch64-none-elf/aarch64-none-elf/include/c++/13.2.1/aarch64-none-elf/)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/lib/tiny_ttf/include)

# LINK_DIRECTORIES(/opt/homebrew/lib)
# LINK_DIRECTORIES(/opt/homebrew/opt/freetype/lib)

set(LV_CONF_INCLUDE_SIMPLE ON)
set(LV_CONF_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lv_conf.h
    CACHE STRING "" FORCE)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/circle/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

add_subdirectory(lib/circle/addon/lvgl/lvgl)
# add_subdirectory(lib/lv_drivers/)

# file(GLOB_RECURSE CPP_SOURCES "src/*.cpp" )
file(GLOB TINY_TTF_INCLUDES "lib/tiny_ttf/include/*")
file(GLOB TINY_TTF_SOURCES "lib/tiny_ttf/src/*")

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(CMAKE_CXX_FLAGS "-O3")

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/src/font.c") 
    include(cmake/bin2h.cmake)

    file(WRITE src/font.c "#include <stddef.h>\n")
    file(GLOB_RECURSE FONTS "fonts/*.ttf")
    foreach(file ${FONTS})
        get_filename_component(variableName ${file} NAME)
        bin2h(SOURCE_FILE ${file} HEADER_FILE src/font.c VARIABLE_NAME ${variableName} APPEND NULL_TERMINATE)
        file(APPEND src/font.c "\n")
    endforeach()
endif()

add_subdirectory(lib/circle)
add_subdirectory(lib/circle/addon/lvgl)
add_subdirectory(src)