#include "random_level.h"
#include <stdio.h>
#include <time.h>

#define MAX_HEIGHT 12
#define LOOP_SIZE 10000000

void run(uint8_t compute(uint8_t), uint64_t *results) {
  float start_time, end_time;
  start_time = (float)clock() / CLOCKS_PER_SEC;

  for (size_t i = 0; i < LOOP_SIZE; ++i) {
    uint8_t level = compute(MAX_HEIGHT);
    ++results[level - 1];
  }

  end_time = (float)clock() / CLOCKS_PER_SEC;
  fprintf(stderr, "ran %d in %fs\n", LOOP_SIZE, end_time - start_time);
}

int main() {
  clock_t c = clock();
  uint64_t state[4] = {1, c, c, 1};
  rng_init(state);

  uint64_t buckets_few[MAX_HEIGHT + 1] = {0};
  uint64_t buckets_mid[MAX_HEIGHT + 1] = {0};
  uint64_t buckets_many[MAX_HEIGHT + 1] = {0};

  run(get_random_level_few, buckets_few);
  run(get_random_level_mid, buckets_mid);
  run(get_random_level_many, buckets_many);

  fprintf(stderr, "\n");
  printf("p,Level,Count,\"#(Level+1)/#Level\"\n");

  for (size_t i = 0; i < MAX_HEIGHT; ++i) {
    double diff = (double)buckets_few[i + 1] / buckets_few[i];
    printf("\"1/4\",%zu,%lu,%f\n", i + 1, buckets_few[i], diff);
  }

  for (size_t i = 0; i < MAX_HEIGHT; ++i) {
    double diff = (double)buckets_mid[i + 1] / buckets_mid[i];
    printf("\"5/16\",%zu,%lu,%f\n", i + 1, buckets_mid[i], diff);
  }

  for (size_t i = 0; i < MAX_HEIGHT; ++i) {
    double diff = (double)buckets_many[i + 1] / buckets_many[i];
    printf("\"1/e\",%zu,%lu,%f\n", i + 1, buckets_many[i], diff);
  }
}
