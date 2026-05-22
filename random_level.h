#ifndef VEIGARIBO_RANDOM_LEVEL
#define VEIGARIBO_RANDOM_LEVEL

#include "rng.h"
#include <stdint.h>

#if defined(__GNUC__)
#include <x86gprintrin.h>
#endif
#if defined(__clang__)
#include <x86intrin.h>
#endif

#define FIX_RANGE (6)
#define FIX_SCALE (32 - FIX_RANGE)

#define FIX_DIV(a, b) (((uint64_t)(a) << 16) / (b) << 10)

#define FIX_ONE (1 << FIX_SCALE)
#define FIX_LOG2_FIVE_OVER_SIXTEEN (0b00000110101101100101100001111011)
#define FIX_LOG2_INV_E (0b00000101110001010101000111011001)
#define FIX_LOG2_ONE_FOURTH (0b00001000000000000000000000000000)

static inline uint8_t leading_zero_count(uint32_t v) {
#if defined(__LZCNT__)

#if defined(__GNUC__)
  return _lzcnt_u32(v);
#endif
#if defined(__clang__)
  return __lzcnt32(v);
#endif

#else
  return 0;
#error "Not good without LZCNT (-mlzcnt)"
#endif
}

static inline uint32_t fix_log2_approx(uint32_t fix_v) {
  uint32_t integral = (leading_zero_count(fix_v << FIX_RANGE));
  uint32_t fix_integral = integral << FIX_SCALE;

  uint32_t fix_mantissa =
      ~(fix_v << (integral + 1)) & (0b00000011111111111111111111111111);

  return fix_integral | fix_mantissa;
}

static inline uint8_t get_random_level(uint32_t fix_log2_p, uint8_t max) {
  uint32_t fix_random = rng_next();
  fix_random = fix_random >> FIX_RANGE;
  uint32_t fix_log_random = fix_log2_approx(fix_random);

  uint32_t fix_result = FIX_ONE + FIX_DIV(fix_log_random, fix_log2_p);
  uint8_t result = (uint8_t)(fix_result >> FIX_SCALE);

  return (result < max) ? result : max;
}

static inline uint8_t get_random_level_few(uint8_t max) {
  return get_random_level(FIX_LOG2_ONE_FOURTH, max);
}

static inline uint8_t get_random_level_mid(uint8_t max) {
  return get_random_level(FIX_LOG2_FIVE_OVER_SIXTEEN, max);
}

static inline uint8_t get_random_level_many(uint8_t max) {
  return get_random_level(FIX_LOG2_INV_E, max);
}

#endif // !VEIGARIBO_RANDOM_LEVEL
