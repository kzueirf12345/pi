#include "Calculation/GaussLegendre.hpp"

namespace pi {

using namespace mpfr;

static size_t digitsToBits(size_t digits) {
    return static_cast<size_t>(digits * 3.32192809488736234787) + 16;
}

mpreal GaussLegendre(size_t decimal_digits) {
    const size_t prec_bits = digitsToBits(decimal_digits);
    
    mpfr_prec_t old_prec = mpreal::get_default_prec();
    mpreal::set_default_prec(prec_bits);

    mpreal a = 1.0;
    mpreal b = 1.0 / sqrt(mpreal(2.0));
    mpreal t = 0.25;
    mpreal p = 1.0;

    size_t iterations = (decimal_digits > 0) ? static_cast<size_t>(std::log2(decimal_digits)) + 5 : 5;

    for (size_t i = 0; i < iterations; ++i) {
        mpreal a_next = (a + b) * 0.5;
        
        mpreal b_next = sqrt(a * b);
        
        mpreal diff = a - a_next;
        mpreal t_next = t - p * (diff * diff);
        
        mpreal p_next = p * 2.0;

        a = a_next;
        b = b_next;
        t = t_next;
        p = p_next;
    }

    mpreal sum = a + b;
    mpreal pi = (sum * sum) / (4.0 * t);

    mpreal::set_default_prec(old_prec);

    return pi;
}

} //namespace pi