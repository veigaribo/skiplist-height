#include <benchmark/benchmark.h>
#include <cstddef>
#include <functional>
#include <thread>

// The one RocksDB uses as of 2026-05-19 (memtable/inlineskiplist.h)

class Random {
private:
  enum : uint32_t {
    M = 2147483647L // 2^31-1
  };
  enum : uint64_t {
    A = 16807 // bits 14, 8, 7, 5, 2, 1, 0
  };

  uint32_t seed_;

  static uint32_t GoodSeed(uint32_t s) { return (s & M) != 0 ? (s & M) : 1; }

public:
  // This is the largest value that can be returned from Next()
  enum : uint32_t { kMaxNext = M };

  explicit Random(uint32_t s) : seed_(GoodSeed(s)) {}

  uint32_t Next() {
    // We are computing
    //       seed_ = (seed_ * A) % M,    where M = 2^31-1
    //
    // seed_ must not be zero or M, or else all subsequent computed values
    // will be zero or M respectively.  For all other values, seed_ will end
    // up cycling through every number in [1,M-1]
    uint64_t product = seed_ * A;

    // Compute (product % M) using the fact that ((x << 31) % M) == x.
    seed_ = static_cast<uint32_t>((product >> 31) + (product & M));
    // The first reduction may overflow by 1 bit, so we may need to
    // repeat.  mod == M is not possible; using > allows the faster
    // sign-bit-based test.
    if (seed_ > M) {
      seed_ -= M;
    }
    return seed_;
  }

  // Returns a Random instance for use by the current thread without
  // additional locking
  static Random *GetTLSInstance();
};

#define STORAGE_DECL static thread_local

template <typename T, std::size_t Align = alignof(T)> struct aligned_storage {
  struct type {
    alignas(Align) unsigned char data[sizeof(T)];
  };
};

#if defined(__GNUC__) && __GNUC__ >= 4
#define LIKELY(x) (__builtin_expect((x), 1))
#define UNLIKELY(x) (__builtin_expect((x), 0))
#else
#define LIKELY(x) (x)
#define UNLIKELY(x) (x)
#endif

Random *Random::GetTLSInstance() {
  STORAGE_DECL Random *tls_instance;
  STORAGE_DECL aligned_storage<Random>::type tls_instance_bytes;

  auto rv = tls_instance;
  if (UNLIKELY(rv == nullptr)) {
    size_t seed = std::hash<std::thread::id>()(std::this_thread::get_id());
    rv = new (&tls_instance_bytes) Random((uint32_t)seed);
    tls_instance = rv;
  }
  return rv;
}

static const int32_t max_height = 12;
static const int32_t branching_factor = 4;
static const uint16_t kMaxPossibleHeight = 32;
static const uint16_t kMaxHeight_ = static_cast<uint16_t>(max_height);
static const uint16_t kBranching_ = static_cast<uint16_t>(branching_factor);
static const uint32_t kScaledInverseBranching_ =
    (Random::kMaxNext + 1) / kBranching_;

int RandomHeight() {
  auto rnd = Random::GetTLSInstance();

  // Increase height with probability 1 in kBranching
  int height = 1;
  while (height < kMaxHeight_ && height < kMaxPossibleHeight &&
         rnd->Next() < kScaledInverseBranching_) {
    height++;
  }
  // TEST_SYNC_POINT_CALLBACK("InlineSkipList::RandomHeight::height", &height);
  // assert(height > 0);
  // assert(height <= kMaxHeight_);
  // assert(height <= kMaxPossibleHeight);
  return height;
}

static void rocksdb_random_level(benchmark::State &state) {
  for (auto _ : state) {
    auto x = RandomHeight();
    benchmark::DoNotOptimize(x);
  }
}
