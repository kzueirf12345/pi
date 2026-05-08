#include "Calculation/MonteCarlo.hpp"

#include <cstdint>
#include <immintrin.h>
#include <iomanip>
#include <iostream>
#include <pthread.h>
#include <random>
#include <sched.h>
#include <unistd.h>
#include <vector>

#include "Measurer/Measurer.hpp"
#include "Calculation/Rand.hpp"

namespace pi {

static inline constexpr const uint64_t BASE_MULTIPLIER = 48271ull;
static inline constexpr const uint64_t MODULUS_POW = 31;
static inline constexpr const uint64_t MODULUS = ((UINT64_C(1) << MODULUS_POW) - 1);
static inline constexpr const uint64_t MODULUS2 = MODULUS * MODULUS;

static uint64_t fast_pow_mod(uint64_t pow) {
    uint64_t res = 1;

    while (pow > 0) {
        if (pow % 2 == 0) {
            res *= res;
            pow /= 2;
        }
        else {
            res *= BASE_MULTIPLIER;
            --pow;
        }
        res %= MODULUS;
    }

    return res;
}

struct PiThreadData {
    size_t iterations = 0;
    uint32_t seed = 0;
    size_t hits = 0;

    pthread_barrier_t* barrier_ptr = nullptr;
    size_t core_id = 0;
};

static std::vector<PiThreadData> GeneratePiThreadsData(
    const size_t cores_cnt, const size_t iterations, const uint32_t seed, pthread_barrier_t* const barrier_ptr
) {
    assert(cores_cnt > 0);
    assert(iterations > 0);

    const size_t iterations_by_thread = iterations / cores_cnt;
    const uint64_t multiplier = fast_pow_mod(iterations_by_thread * 2);
    
    std::vector<PiThreadData> threads_data(
        cores_cnt, 
        {.iterations = iterations_by_thread, .seed = seed, .hits = 0, .barrier_ptr = barrier_ptr, .core_id = 0}
    );

    threads_data[0].iterations += iterations % cores_cnt;

    for (size_t thread_num = 1; thread_num < cores_cnt; ++thread_num) {
        threads_data[thread_num].seed = static_cast<uint32_t>(
            (static_cast<uint64_t>(threads_data[thread_num - 1].seed) * multiplier) % MODULUS
        );
        threads_data[thread_num].core_id = thread_num;
    }

    return threads_data;
}

template<typename T>
concept VectorGenerator = requires(T gen) {
    { gen() };
    requires std::is_same_v<decltype(gen()), __m512i>;
};

template<typename T>
concept ScalarGenerator = requires(T gen) {
    { gen() } -> std::convertible_to<uint32_t>;
};

template<typename T>
concept Generator = ScalarGenerator<T> || VectorGenerator<T>;


template <Generator GenT>
static void* PiThreadFoo(void* arg) {
    PiThreadData* data = static_cast<PiThreadData*>(arg);

    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(data->core_id, &cpuset);
    pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);

    GenT gen(data->seed);
    size_t local_hits = 0;

    pthread_barrier_wait(data->barrier_ptr);

    if constexpr (VectorGenerator<GenT>) {
        assert(data->iterations % 8 == 0);

        const static __m512i modulus2_vec = _mm512_set1_epi64(MODULUS2);

        const size_t iterations = data->iterations / 8;

        for (size_t iteration = 0; iteration < iterations; ++iteration) {
            const __m512i vec = gen();

            const __m512i x2_vec = _mm512_mul_epu32(vec, vec);
            const __m512i vec_shifted = _mm512_srli_epi64(vec, 32);
            const __m512i y2_vec  = _mm512_mul_epu32(vec_shifted, vec_shifted);

            const __m512i dist2_vec = _mm512_add_epi64(x2_vec, y2_vec);

            const __mmask8 mask = _mm512_cmple_epu64_mask(dist2_vec, modulus2_vec);

            local_hits += _mm_popcnt_u32(mask);
        }
    }
    else if constexpr (ScalarGenerator<GenT>) {
        for (size_t iteration = 0; iteration < data->iterations; ++iteration) {
            const uint64_t x = static_cast<uint64_t>(gen());
            const uint64_t y = static_cast<uint64_t>(gen());

            local_hits += (x * x + y * y <= MODULUS2);
        }
    }
    
    data->hits = local_hits;

    return data;
}

template <Generator GenT>
void SetupPiFunc(
    const size_t cores_cnt, const size_t iterations, const uint32_t seed, 
    pthread_barrier_t* const barrier_ptr,
    std::vector<pthread_t>& threads, std::vector<PiThreadData>& threads_data
) {
    pthread_barrier_init(barrier_ptr, nullptr, cores_cnt + 1);

    threads_data = GeneratePiThreadsData(cores_cnt, iterations, seed, barrier_ptr);

    for (size_t thread_num = 0; thread_num < cores_cnt; ++thread_num) {
        pthread_create(
            &threads[thread_num], nullptr, PiThreadFoo<GenT>, &threads_data[thread_num]
        );
    }

    pthread_barrier_wait(barrier_ptr);
}

size_t DoPiFunc(
    const size_t cores_cnt, 
    const std::vector<pthread_t>& threads, std::vector<PiThreadData>& threads_data
) {
    size_t hits = 0;
    for (size_t thread_num = 0; thread_num < cores_cnt; ++thread_num) {
        pthread_join(threads[thread_num], nullptr);
        hits += threads_data[thread_num].hits;
    }
    return hits;
}

template <Generator GenT>
measurer::Val BenchSomeGen(
    const size_t cores_cnt, 
    const size_t buckets, const size_t batches, const size_t iterations, 
    const uint32_t seed
) {
    pthread_barrier_t barrier{};
    pthread_barrier_t* const barrier_ptr = &barrier;

    std::vector<pthread_t> threads(cores_cnt);
    std::vector<PiThreadData> threads_data{};

    auto setup_func = [cores_cnt, iterations, seed, barrier_ptr, &threads, &threads_data](){
        SetupPiFunc<GenT>(cores_cnt, iterations, seed, barrier_ptr, threads, threads_data);
    };
    auto do_func = [cores_cnt, iterations, &threads, &threads_data](){
        return 4. * DoPiFunc(cores_cnt, threads, threads_data) / iterations;
    };

    const measurer::Val time = measurer::Runner::benchLatency(buckets, batches, setup_func, do_func);

    pthread_barrier_destroy(barrier_ptr);

    return time;
}


void BenchMonteCarlo(
    std::ostream& out, uint32_t seed, 
    const size_t buckets, const size_t batches, size_t iterations
) {
    const size_t cores_cnt = sysconf(_SC_NPROCESSORS_ONLN);

    const measurer::Val res_scalar = BenchSomeGen<minstd_rand::MinstdRand>      (
        cores_cnt, buckets, batches, iterations, seed
    );
    const measurer::Val    res_std = BenchSomeGen<std::minstd_rand>(
        cores_cnt, buckets, batches, iterations, seed
    );
    const measurer::Val    res_vec = BenchSomeGen<minstd_rand::MinstdRandVec>   (
        cores_cnt, buckets, batches, iterations, seed
    );

    auto print_row = [&out](const std::string& name, const measurer::Val res) {
        out << std::left << std::setw(25) << name 
            << "Time: " << std::right << std::setw(15) 
            << static_cast<uint64_t>(res.mean) << " +/- " 
            << static_cast<uint64_t>(res.stddev) << " clks\n";
    };

    out << "\n===Pi Benchmark===\n";
    print_row("std::minstd_rand", res_std);
    print_row("My MinstdRand", res_scalar);
    print_row("My MinstdRandVec", res_vec);
}

}
