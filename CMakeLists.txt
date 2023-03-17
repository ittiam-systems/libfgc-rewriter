cmake_minimum_required(VERSION 3.9.1)
project(libfgc_rewriter)

set(FGCR_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(FGCR_CONFIG_DIR "${CMAKE_CURRENT_BINARY_DIR}")

if("${FGCR_ROOT}" STREQUAL "${FGCR_CONFIG_DIR}")
  message(
    FATAL_ERROR
      "Building from within the libfgc_rewriter source tree is not supported.\n"
      "Hint: Run these commands\n"
      "$ rm -rf CMakeCache.txt CMakeFiles\n"
      "$ mkdir -p ./build\n"
      "$ cd ./build\n"
      "And re-run CMake from the build directory.")
endif()

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
    add_library("${projName}CompileOptions" INTERFACE)
    target_compile_options(
      "${projName}CompileOptions"
      INTERFACE /INCREMENTAL:NO
                /Oi
                /GL
                /Os
                /MP
                /W4
                /WX)
endif()

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
    target_compile_definitions("${projName}CompileOptions" 
      INTERFACE WINDOWS _CRT_SECURE_NO_WARNINGS)
    add_definitions(-DWIN32 -D_WIN32 -DWINDOWS -D_WINDOWS -DWINDOWS_TIMER -DMSVC -D_CRT_SECURE_NO_WARNINGS)
endif()

add_definitions(-DPROFILE_ENABLE)

include("${FGCR_ROOT}/rewriter/rewriter.cmake")
include("${FGCR_ROOT}/test/test.cmake")