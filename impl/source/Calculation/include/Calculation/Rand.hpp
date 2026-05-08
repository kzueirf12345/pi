#pragma once

#include <cassert>
#include <concepts>
#include <cstdint>
#include <immintrin.h>

#ifdef _MSC_VER
    #include <intrin.h>
#else
    #include <x86intrin.h>
#endif

namespace minstd_rand {

class MinstdRand {

private:

    static inline constexpr const uint32_t MULTIPLIER_ = UINT32_C(48271);
    static inline constexpr const uint32_t MODULUS_POW_ = 31;
    static inline constexpr const uint32_t MODULUS_ = ((UINT32_C(1) << MODULUS_POW_) - 1);

public:

    MinstdRand() noexcept;

    MinstdRand(uint32_t seed);

    uint32_t operator()() noexcept;

    template <std::floating_point T>
    T Get01() noexcept {
        constexpr const T INV_MODULUS = static_cast<T>(1) / static_cast<T>(MODULUS_);
        return static_cast<T>(operator()()) * INV_MODULUS;
    }

private:

    uint32_t prev_val_ = 1;

};

class MinstdRandVec {

private:

    static inline constexpr const uint32_t MULTIPLIER_ = UINT32_C(1098894339);
    static inline constexpr const uint32_t MODULUS_POW_ = 31;
    static inline constexpr const uint32_t MODULUS_ = ((UINT32_C(1) << MODULUS_POW_) - 1);
    static inline constexpr const float    INV_MODULUS_ = 1.f / static_cast<float>(MODULUS_);

    static inline constexpr const uint64_t ALIGNMENT_ = 64;
    static inline constexpr const size_t VALS_IN_VEC_ = 16;
    
    alignas(ALIGNMENT_) static inline constexpr const uint32_t INIT_VALS_[VALS_IN_VEC_] = {
        48271,
        182605794,
        1291394886,
        1914720637,
        2078669041,
        407355683,
        1105902161,
        854716505,
        564586691,
        1596680831,
        192302371,
        1203428207,
        1250328747,
        1738531149,
        1271135913,
        1098894339,
    };

    static inline const __m512i multipliers_vec_ = _mm512_set1_epi32(MULTIPLIER_);
    static inline const __m512i modulus_vec_ = _mm512_set1_epi64(MODULUS_);
    static inline const __m512 inv_modulus_vec_ =  _mm512_set1_ps(INV_MODULUS_);

public:

    MinstdRandVec();

    MinstdRandVec(uint32_t seed);

    __m512i operator()();

    __m512 Getf01();

private:

    __m512i prev_vals_ = _mm512_load_si512(INIT_VALS_);

};


} // namespace minstd_rand