cmake_minimum_required (VERSION 3.3)

#[[
set (CMAKE_C_COMPILER   "/usr/local/opt/llvm/bin/clang")
set (CMAKE_CXX_COMPILER "/usr/local/opt/llvm/bin/clang++")
]]

project (maze-viewer VERSION 0.1 LANGUAGES C CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build (Debug, Release, MinSizeRel, RelWithDebInfo)" FORCE)
endif()

add_subdirectory (lib)

add_custom_target(run 
	COMMAND love .
	DEPENDS lib)

