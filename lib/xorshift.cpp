#include "xorshift.hpp"

#include <cstdlib>
#include <cstring>
#include <cstdarg>

xorshift_t *
xorshift_create(void)
{
	xorshift_t *state = (xorshift_t *)malloc(sizeof(xorshift_t));
	state->x = 0x193a6754;
	state->y = 0xa8a7d469;
	state->z = 0x97830e05;
	state->w = 0x113ba7bb;

	return state;
}

void
xorshift_reseed(xorshift_t *state, ...)
{
	va_list args;
	va_start(args, state);
	state->x = va_arg(args, uint32_t);
	state->y = va_arg(args, uint32_t);
	state->z = va_arg(args, uint32_t);
	state->w = va_arg(args, uint32_t);
	va_end(args);
}

xorshift_t *
xorshift_clone(xorshift_t *state)
{
	xorshift_t *clone = (xorshift_t *)malloc(sizeof(xorshift_t));
	memcpy(clone, state, sizeof(xorshift_t));

	return clone;
}

uint32_t
xorshift_u32(xorshift_t *s)
{
	uint32_t x = s->x;
	uint32_t t = x ^ (x << 11);
	s->x = s->y;
	s->y = s->z;
	s->z = s->w;
	uint32_t w = s->w;
	s->w = w ^ (w >> 19) ^ (t ^ (t >> 8));

	return s->w;
}

uint64_t
xorshift_u64(xorshift_t *state)
{
	return ((uint64_t)xorshift_u32(state)) << 32 | ((uint64_t)xorshift_u32(state));
}

float
xorshift_float(xorshift_t *state)
{
	// static const uint32_t MANTISSA_BITS = 24;
	static const uint32_t IGNORED_BITS = 8;
	static const double SCALE = 0x1000000; // 1 << MANTISSA_BITS

	return (float)(xorshift_u32(state) >> IGNORED_BITS) / SCALE;
}

float
xorshift_float_range(xorshift_t *state, float min, float max)
{
	return min + xorshift_float(state) * (max - min);
}

double
xorshift_double(xorshift_t *state)
{
	// static const uint64_t MANTISSA_BITS = 53;
	static const uint64_t IGNORED_BITS = 11;
	static const double SCALE = 0x20000000000000; // 1 << MANTISSA_BITS

	return (double)(xorshift_u64(state) >> IGNORED_BITS) / SCALE;
}

double
xorshift_double_range(xorshift_t *state, double min, double max)
{
	return min + xorshift_double(state) * (max - min);
}
