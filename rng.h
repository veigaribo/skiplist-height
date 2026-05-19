#include <stdint.h>

// https://prng.di.unimi.it/
// https://prng.di.unimi.it/xoshiro256plus.c
void rng_init(uint64_t state[4]);
uint64_t rng_next(void);
