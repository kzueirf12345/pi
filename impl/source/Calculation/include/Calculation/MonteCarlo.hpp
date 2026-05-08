#pragma once

#include <cstddef>
#include <immintrin.h>
#include <ostream>

namespace pi {

void BenchMonteCarlo(
    std::ostream& out, uint32_t seed, 
    const size_t buckets, const size_t batches, size_t iterations
);

} // namespace pi