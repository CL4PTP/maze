#pragma once

#include <cstdint>

#include "xorshift.hpp"

typedef struct maze_raw_t
{
	uint64_t width;
	uint64_t height;
	uint32_t arr[1]; //marker for the rest of the array
} maze_raw_t;

typedef struct maze_t
{
	int fd;
	maze_raw_t *raw;
} maze_t;

typedef enum maze_generation_t
{
	Sidewinder,
	NaiveSidewinder
} maze_generation_t;

typedef enum maze_cardinal_directions_t
{
	S = 1,
	E = 2,
	N = 4,
	W = 8
} maze_cardinal_directions_t;

extern "C"
{

maze_t *
maze_create(const char *filepath, uint64_t width, uint64_t height);

maze_t *
maze_open(const char *filepath);

void
maze_free(maze_t *maze);

void
maze_generate(maze_t *maze, maze_generation_t type);

void
maze_generate_sidewinder(maze_t *maze);

void
maze_generate_naive_sidewinder(maze_t *maze);

uint32_t
retrieve_bits(uint32_t value, uint32_t nth);

uint32_t
prepare_bits(uint32_t value, uint32_t nth);

void
maze_raw_or_set(maze_raw_t *raw, uint64_t index, uint8_t value);

void
maze_or_set(maze_t *maze, uint64_t x, uint64_t y, uint8_t value);

uint8_t
maze_raw_get(maze_raw_t *raw, uint64_t index);

uint8_t
maze_get(maze_t *maze, uint64_t x, uint64_t y);

bool
maze_test(maze_t *maze, uint64_t x, uint64_t y, maze_cardinal_directions_t direction);

} // extern "C"
