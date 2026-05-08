#include "Calculation/Chudnovsky.hpp"

#include <gmpxx.h>

namespace pi {

using namespace mpfr;

static size_t digitsToBits(size_t digits) {
    return static_cast<size_t>(digits * 3.32192809488736234787) + 16;
}

void ChudnovskyBinarySplitting(uint32_t a, uint32_t b, mpz_class& P, mpz_class& Q, mpz_class& T) {
    static const mpz_class CONST_C3_OVER_24("10939058860032000");
    static const mpz_class CONST_A("545140134");
    static const mpz_class CONST_B("13591409");

    if (b - a == 1) {
        if (a == 0) {
            P = 1;
            Q = 1;
            T = CONST_B;
        } else {
            mpz_class a_z = a;
            
            P = -(6 * a_z - 5) * (2 * a_z - 1) * (6 * a_z - 1);
            
            Q = CONST_C3_OVER_24 * (a_z * a_z * a_z);
            
            T = P * (CONST_A * a_z + CONST_B);
        }
        return;
    }

    uint32_t m = a + (b - a) / 2;
    
    mpz_class P1, Q1, T1;
    mpz_class P2, Q2, T2;

    ChudnovskyBinarySplitting(a, m, P1, Q1, T1);
    ChudnovskyBinarySplitting(m, b, P2, Q2, T2);

    P = P1 * P2;
    Q = Q1 * Q2;
    T = T1 * Q2 + P1 * T2;
}


mpreal Chudnovsky(size_t decimal_digits) {

    const size_t prec_bits = digitsToBits(decimal_digits);
    mpfr_prec_t old_prec = mpreal::get_default_prec();
    mpreal::set_default_prec(prec_bits);

    size_t N = (decimal_digits / 14.181647462725477) + 2; 

    mpz_class P, Q, T;
    ChudnovskyBinarySplitting(0, N, P, Q, T);

    mpreal Q_float(Q.get_mpz_t());
    mpreal T_float(T.get_mpz_t());
    mpreal C_float = 640320.0;

    mpreal pi = (C_float * sqrt(C_float) * Q_float) / (12.0 * T_float);

    mpreal::set_default_prec(old_prec);

    return pi;
}

} //namespace pi