#include "Calculation/GaussLegendre.hpp"

namespace pi {

using namespace mpfr;

static size_t digitsToBits(size_t digits) {
    return static_cast<size_t>(digits * 3.32192809488736234787) + 16;
}

mpfr::mpreal GaussLegendre(size_t decimal_digits) {
    mpreal::set_default_prec(digitsToBits(decimal_digits));
    
    //TODO implement
    return mpreal("3.14159265358979323846264338327950288419716939937510");
}

} //namespace pi