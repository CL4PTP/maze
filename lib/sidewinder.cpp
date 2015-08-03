#include "sidewinder.h"

#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

typedef struct sidewinder_chunk_info_t
{
	maze_t *maze;
	xorshift_t *rng;
	uint64_t start;
	uint64_t end;
} sidewinder_chunk_info_t;

static void *
maze_generate_sidewinder_process_chunk(void *_chunk_info)
{
	sidewinder_chunk_info_t *chunk_info = (sidewinder_chunk_info_t *)_chunk_info;
	maze_t *maze = chunk_info->maze;

	uint64_t width = maze->raw->width;
	uint64_t start = chunk_info->start;
	uint64_t end = chunk_info->end;

	for (uint64_t y = start; y < end; ++y)
	{
		uint64_t run_start = 0;

		for (uint64_t x = 0; x < width; ++x)
		{
			if (y > 0 && (x + 1 == width || xorshift_float(chunk_info->rng) >= 0.5))
			{
				uint64_t carve_point =
					run_start + (uint64_t)xorshift_float_range(chunk_info->rng, 0, x - run_start);

				maze_or_set(maze, carve_point, y - 1, S);
				run_start = x + 1;
			}
			else if (x + 1 < width)
			{
				maze_or_set(maze, x, y, E);
			}
		}
	}

	return NULL;
}

void
maze_generate_sidewinder_inner(maze_t *maze)
{
	const int NUM_CORES = sysconf(_SC_NPROCESSORS_ONLN) * 2;
	const uint64_t ROWS_PER_CHUNK = maze->raw->height / NUM_CORES;

	assert(maze->raw->height % NUM_CORES == 0);

	pthread_t *threads = (pthread_t *)malloc(sizeof(pthread_t) * NUM_CORES);
	sidewinder_chunk_info_t *chunk_info =
		(sidewinder_chunk_info_t *)malloc(sizeof(sidewinder_chunk_info_t) * NUM_CORES);
	xorshift_t *rng = xorshift_create();

	for (int i = 0; i < NUM_CORES; ++i)
	{
		uint32_t new_seed[4] = {
			xorshift_u32(rng), xorshift_u32(rng), xorshift_u32(rng), xorshift_u32(rng)
		};
		xorshift_reseed(rng, new_seed[3], new_seed[0], new_seed[1], new_seed[2]);

		chunk_info[i].maze = maze;
		chunk_info[i].start = ROWS_PER_CHUNK * i;
		chunk_info[i].end = ROWS_PER_CHUNK * (i + 1);
		chunk_info[i].rng = xorshift_clone(rng);
		int result =
			pthread_create(&threads[i], NULL, maze_generate_sidewinder_process_chunk, (void *)&chunk_info[i]);

		if(result)
		{
			perror("Failed to create thread");
			exit(EXIT_FAILURE);
		}
	}

	for (int i = 0; i < NUM_CORES; ++i)
	{
		int result = pthread_join(threads[i], NULL);

		if (result)
		{
			perror("Failed to join thread");
			exit(EXIT_FAILURE);
		}

		free(chunk_info[i].rng);
	}

	free(rng);
	free(threads);
	free(chunk_info);
}
