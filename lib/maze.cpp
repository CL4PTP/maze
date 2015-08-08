#include "maze.hpp"
#include "sidewinder.hpp"

#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif

maze_t *
maze_create(const char *filepath, uint64_t width, uint64_t height)
{
	int result;
	int fd;

	fd = open(filepath, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0644);
	if (fd == -1)
	{
		perror("Error opening file for writing");
		exit(EXIT_FAILURE);
	}

	write(fd, &width, sizeof(width));
	write(fd, &height, sizeof(height));
	lseek(fd, ((width * height) / 4) - 1, SEEK_CUR);
	result = write(fd, "", 1);
	if (result == -1)
	{
		close(fd);
		perror("Error writing to file");
		exit(EXIT_FAILURE);
	}

	close(fd);

	return maze_open(filepath);
}

maze_t *
maze_open(const char *filepath)
{
	int result;
	int fd;
	uint64_t width, height;
	void *map;
	maze_t *maze;
	uint64_t maze_size;

	fd = open(filepath, O_RDWR, (mode_t)0644);
	if (fd == -1) {
		perror("Error opening file for writing");
		exit(EXIT_FAILURE);
	}

	result = read(fd, &width, sizeof(width));
	result = read(fd, &height, sizeof(height));
	if (result == -1)
	{
		close(fd);
		perror("Error reading file");
		exit(EXIT_FAILURE);
	}

	maze_size = sizeof(width) + sizeof(height) + ((width * height) / 4);

	map = mmap(0, maze_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (map == MAP_FAILED)
	{
		close(fd);
		perror("Error mmapping file");
		exit(EXIT_FAILURE);
	}

	maze = (maze_t *)malloc(sizeof(maze_t));
	maze->fd = fd;
	maze->raw = (maze_raw_t *)map;

	return maze;
}

void
maze_free(maze_t *maze)
{
	uint64_t maze_size = sizeof(maze->raw->width) + sizeof(maze->raw->height) +
		((maze->raw->width * maze->raw->height) / 4);

	if (munmap(maze->raw, maze_size) == -1)
	{
		perror("Error un-mmapping the file");
	}

	close(maze->fd);
	free(maze);
}

void
maze_generate(maze_t *maze, maze_generation_t type)
{
	switch(type)
	{
		case Sidewinder:
			maze_generate_sidewinder(maze);
			break;
		case NaiveSidewinder:
			maze_generate_naive_sidewinder(maze);
			break;
		default:
			fprintf(stderr, "Invalid maze type provided: %i\n", (int)type);
	}
}

void
maze_generate_sidewinder(maze_t *maze)
{
	maze_generate_sidewinder_inner(maze);
}

void
maze_generate_naive_sidewinder(maze_t *maze)
{
	xorshift_t *rng = xorshift_create();

	maze_raw_t *raw = maze->raw;

	for (uint64_t y = 0; y < raw->height; ++y)
	{
		uint64_t run_start = 0;

		for (uint64_t x = 0; x < raw->width; ++x)
		{
			if (y > 0 && (x + 1 == raw->width || xorshift_float(rng) >= 0.5))
			{
				uint64_t carve_point =
					run_start + (uint64_t)xorshift_float_range(rng, 0, x - run_start);

				maze_or_set(maze, carve_point, y - 1, S);
				run_start = x + 1;
			}
			else if (x + 1 < raw->width)
			{
				maze_or_set(maze, x, y, E);
			}
		}
	}

	free(rng);
}

uint32_t
retrieve_bits(uint32_t value, uint32_t nth)
{
	return (value >> (nth << 1)) & 0x3; // 0x3 = 0b11
}

uint32_t
prepare_bits(uint32_t value, uint32_t nth)
{
	return (value & 0x3) << (nth << 1);
}

void
maze_raw_or_set(maze_raw_t *raw, uint64_t index, uint8_t value)
{
	raw->arr[index / 16] |= prepare_bits(value, index % 16);
}

void
maze_or_set(maze_t *maze, uint64_t x, uint64_t y, uint8_t value)
{
	maze_raw_or_set(maze->raw, x + y * maze->raw->width, value);
}

uint8_t
maze_raw_get(maze_raw_t *raw, uint64_t index)
{
	return retrieve_bits(raw->arr[index / 16], index % 16);
}

uint8_t
maze_get(maze_t *maze, uint64_t x, uint64_t y)
{
	return maze_raw_get(maze->raw, x + y * maze->raw->width);
}

bool
maze_test(maze_t *maze, uint64_t x, uint64_t y, maze_cardinal_directions_t direction)
{
	return maze_get(maze, x, y) & direction;
}
