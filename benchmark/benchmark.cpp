#include "random_level.h"
#include <benchmark/benchmark.h>
#include <benchmark/export.h>

#include "redis.cpp"
#include "rocksdb.cpp"

// Basic repeated check implementation:

static inline uint64_t rotl(const uint64_t x, int k) {
  return (x << k) | (x >> (64 - k));
}

// xoshiro256++ is better than xoshiro256+ in this case
uint64_t basic_s[4] = {1, 2, 3, 4};

uint64_t basic_rng_next(void) {
  const uint64_t result = rotl(basic_s[0] + basic_s[3], 23) + basic_s[0];
  const uint64_t t = basic_s[1] << 17;

  basic_s[2] ^= basic_s[0];
  basic_s[3] ^= basic_s[1];
  basic_s[1] ^= basic_s[2];
  basic_s[0] ^= basic_s[3];

  basic_s[2] ^= t;
  basic_s[3] = rotl(basic_s[3], 45);
  return result;
}

uint8_t basic_random_level(uint8_t max) {
  uint8_t level = 1;
  uint64_t random = basic_rng_next();

  // 5/16 chance
  while ((random & 0xf) > 10) {
    ++level;
    random = random >> 4;
  }

  if (level < max) {
    return level;
  } else {
    return max;
  }
}

static void basic_random_level(benchmark::State &state) {
  for (auto _ : state) {
    basic_random_level(16);
  }
}

// My implementation:

static void veigaribo_random_level(benchmark::State &state) {
  for (auto _ : state) {
    get_random_level_mid(16);
  }
}

// Register the function as a benchmark
BENCHMARK(basic_random_level);
BENCHMARK(redis_random_level);
BENCHMARK(rocksdb_random_level);
BENCHMARK(veigaribo_random_level);

// Run the benchmark
BENCHMARK_MAIN();
