#include "stdlib.h"
#include <benchmark/benchmark.h>

// The one Redis uses as of 2026-05-19:

#define ZSKIPLIST_MAXLEVEL 32
#define ZSKIPLIST_P 0.3125
#define ZSKIPLIST_MAX_SEARCH 10

// From t_zset.c:
static int zslRandomLevel(void) {
  static const int threshold = ZSKIPLIST_P * RAND_MAX;
  int level = 1;
  while (random() < threshold)
    level += 1;
  return (level < ZSKIPLIST_MAXLEVEL) ? level : ZSKIPLIST_MAXLEVEL;
}

static void redis_random_level(benchmark::State &state) {
  for (auto _ : state) {
    zslRandomLevel();
  }
}
