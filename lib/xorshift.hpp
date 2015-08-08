#pragma once

#include <cstdint>

/*
	xorshift lifted from http://doc.rust-lang.org/rand/src/rand/lib.rs.html
--------------------------------------------------------------------------------
*/

typedef struct xorshift_t
{
	uint32_t x;
	uint32_t y;
	uint32_t z;
	uint32_t w;
} xorshift_t;

extern "C"
{

xorshift_t *
xorshift_create(void);

void
xorshift_reseed(xorshift_t *state, ...);

xorshift_t *
xorshift_clone(xorshift_t *state);

uint32_t
xorshift_u32(xorshift_t *);

uint64_t
xorshift_u64(xorshift_t *state);

float
xorshift_float(xorshift_t *state);

float
xorshift_float_range(xorshift_t *state, float min, float max);

double
xorshift_double(xorshift_t *state);

double
xorshift_double_range(xorshift_t *state, double min, double max);

} // extern "C"
