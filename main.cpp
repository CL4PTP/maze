#include "lib/maze.hpp"
#include <stdio.h>
#include <cstdlib>

int main()
{
	maze_t *maze = maze_create("/Users/bence/dev/maze_32768x32768.bin", 32768, 32768);
	// maze_t *maze = maze_open("/Users/bence/dev/maze_32768x32768.bin");

	maze_generate(maze, Sidewinder);

	maze_free(maze);

	return EXIT_SUCCESS;
}
