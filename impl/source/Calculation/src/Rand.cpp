#include "Calculation/Rand.hpp"

namespace minstd_rand {

MinstdRand::MinstdRand() noexcept {}

MinstdRand::MinstdRand(uint32_t seed)
    :   prev_val_(seed)
{
    assert(seed % MODULUS_ != 0 && "Invalid state");
}

uint32_t MinstdRand::operator()() noexcept {
    const uint64_t product = (uint64_t)MULTIPLIER_ * prev_val_;
    const uint32_t lo = (uint32_t)product & MODULUS_;
    const uint32_t hi = (uint32_t)(product >> MODULUS_POW_);
    const uint32_t sum = lo + hi;

    const uint32_t sum_lo = sum & MODULUS_;
    const uint32_t sum_hi = sum >> MODULUS_POW_;

    prev_val_ = sum_hi + sum_lo;

    return prev_val_;
}

MinstdRandVec::MinstdRandVec() {}

MinstdRandVec::MinstdRandVec(uint32_t seed)
{
    alignas(ALIGNMENT_) uint32_t arr[VALS_IN_VEC_];
    MinstdRand gen(seed);
    for (size_t ind = 0; ind < VALS_IN_VEC_; ++ind) {
        arr[ind] = gen();
    }
    prev_vals_ = _mm512_load_si512(arr);
}

__m512i MinstdRandVec::operator()() {
    const __m512i prod_even = _mm512_mul_epu32(prev_vals_, multipliers_vec_);
    const __m512i prev_vals_shifted = _mm512_srli_epi64(prev_vals_, 32);
    const __m512i prod_odd  = _mm512_mul_epu32(prev_vals_shifted, multipliers_vec_);

    const __m512i lo_even = _mm512_and_si512(prod_even, modulus_vec_);
    const __m512i hi_even = _mm512_srli_epi64(prod_even, MODULUS_POW_);
    const __m512i sum_even = _mm512_add_epi64(lo_even, hi_even);

    const __m512i lo_odd = _mm512_and_si512(prod_odd, modulus_vec_);
    const __m512i hi_odd = _mm512_srli_epi64(prod_odd, MODULUS_POW_);
    const __m512i sum_odd = _mm512_add_epi64(lo_odd, hi_odd);

    const __m512i sum_lo_even = _mm512_and_si512(sum_even, modulus_vec_);
    const __m512i sum_hi_even = _mm512_srli_epi64(sum_even, MODULUS_POW_);
    const __m512i res_even = _mm512_add_epi64(sum_lo_even, sum_hi_even);

    const __m512i sum_lo_odd = _mm512_and_si512(sum_odd, modulus_vec_);
    const __m512i sum_hi_odd = _mm512_srli_epi64(sum_odd, MODULUS_POW_);
    const __m512i res_odd = _mm512_add_epi64(sum_lo_odd, sum_hi_odd);

    const __m512i res_odd_shifted = _mm512_slli_epi64(res_odd, 32);

    const __m512i res = prev_vals_;

    prev_vals_ = _mm512_or_si512(res_even, res_odd_shifted);

    return res;
}

__m512 MinstdRandVec::Getf01() {
    const __m512i i_vals = operator()();
    const __m512 f_vals = _mm512_cvtepi32_ps(i_vals);
    return _mm512_mul_ps(f_vals, inv_modulus_vec_);
}

} // namespace minstd_rand