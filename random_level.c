#include "random_level.h"
#include "rng.h"

uint8_t get_random_level_few(uint8_t max) {
  return get_random_level(FIX_LOG2_ONE_FOURTH, max);
}

uint8_t get_random_level_mid(uint8_t max) {
  return get_random_level(FIX_LOG2_FIVE_OVER_SIXTEEN, max);
}

uint8_t get_random_level_many(uint8_t max) {
  return get_random_level(FIX_LOG2_INV_E, max);
}
