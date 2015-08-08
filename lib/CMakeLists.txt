#[[
set (CMAKE_AR           "/usr/local/opt/llvm/bin/llvm-ar")
set (CMAKE_LINKER       "/usr/local/opt/llvm/bin/llvm-link")
set (CMAKE_NM           "/usr/local/opt/llvm/bin/llvm-nm")
set (CMAKE_OBJDUMP      "/usr/local/opt/llvm/bin/llvm-objdump")
set (CMAKE_RANLIB       "/usr/local/opt/llvm/bin/llvm-ranlib")
]]

if (NOT MSVC)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -Wpedantic -flto")
	set (CMAKE_CXX_FLAGS_RELEASE "-Ofast -DNDEBUG")
	set (CMAKE_CXX_FLAGS_DEBUG   "-g3 -DDEBUG")
	set (CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG")
	set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-Og -g3")
endif()

set (LIB_SOURCES maze.cpp sidewinder.cpp xorshift.cpp)

add_library(maze SHARED ${LIB_SOURCES})
add_library(maze_static STATIC ${LIB_SOURCES})

find_package (Threads)

target_link_libraries (maze ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries (maze_static ${CMAKE_THREAD_LIBS_INIT})
