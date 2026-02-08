#include "MurmurHash3.h"

static inline std::uint32_t rotl32(std::uint32_t x, int r) {
    return (x << r) | (x >> (32 - r));
}

static inline std::uint32_t fmix32(std::uint32_t h) {
    h ^= h >> 16;
    h *= 0x85ebca6bU;
    h ^= h >> 13;
    h *= 0xc2b2ae35U;
    h ^= h >> 16;
    return h;
}

std::uint32_t murmur3_32(const void* key, std::size_t len, std::uint32_t seed) {
    const std::uint8_t* data = static_cast<const std::uint8_t*>(key);
    const int nblocks = static_cast<int>(len / 4);

    std::uint32_t h1 = seed;
    const std::uint32_t c1 = 0xcc9e2d51U;
    const std::uint32_t c2 = 0x1b873593U;

    const std::uint32_t* blocks = reinterpret_cast<const std::uint32_t*>(data + nblocks * 4);
    for (int i = -nblocks; i; i++) {
        std::uint32_t k1 = blocks[i];
        k1 *= c1;
        k1 = rotl32(k1, 15);
        k1 *= c2;

        h1 ^= k1;
        h1 = rotl32(h1, 13);
        h1 = h1 * 5U + 0xe6546b64U;
    }

    const std::uint8_t* tail = data + nblocks * 4;
    std::uint32_t k1 = 0;

    switch (len & 3) {
    case 3: k1 ^= static_cast<std::uint32_t>(tail[2]) << 16; [[fallthrough]];
    case 2: k1 ^= static_cast<std::uint32_t>(tail[1]) << 8; [[fallthrough]];
    case 1: k1 ^= static_cast<std::uint32_t>(tail[0]);
        k1 *= c1; k1 = rotl32(k1, 15); k1 *= c2; h1 ^= k1;
    }

    h1 ^= static_cast<std::uint32_t>(len);
    h1 = fmix32(h1);
    return h1;
}
