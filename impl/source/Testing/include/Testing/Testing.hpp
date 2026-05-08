#pragma once

#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <cstring>

#include "libs/mpreal/mpreal.h"

namespace pi {

using PiAlgoFunc = mpfr::mpreal(*)(size_t);

void VerifyAlgorithm(
    std::ostream& out, const std::string& algo_name, PiAlgoFunc algo, size_t decimal_digits
);

void BenchAlgorithm(
    std::ostream& out,
    const std::string& algo_name, PiAlgoFunc algo, 
    size_t decimal_digits, size_t buckets, size_t batches
);

} //namepsace pi