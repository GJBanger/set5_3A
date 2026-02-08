#pragma once
#include <cstdint>
#include <cstddef>

std::uint32_t murmur3_32(const void* key, std::size_t len, std::uint32_t seed);
