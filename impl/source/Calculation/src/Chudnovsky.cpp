#include "Calculation/Chudnovsky.hpp"

namespace pi {

using namespace mpfr;

static size_t digitsToBits(size_t digits) {
    return static_cast<size_t>(digits * 3.32192809488736234787) + 16;
}


mpreal Chudnovsky(size_t decimal_digits) {
    mpreal::set_default_prec(digitsToBits(decimal_digits));

    //TODO implement
    
    return mpfr::const_pi();
}

} //namespace pi