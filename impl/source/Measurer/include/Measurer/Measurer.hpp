#pragma once

#include <cmath>
#include <cstdint>
#include <limits>
#include <cassert>
#include <vector>

#ifdef _MSC_VER
    #include <intrin.h>
#else
    #include <x86intrin.h>
#endif

namespace measurer {

template <typename T>
static inline void DoNotOptimizeAway(T&& val) {
    asm volatile("" : : "g"(&val) : "memory");
}

struct Val {
    double mean;
    double stddev;
};

class Runner {

public:

    static void VoidSetup() {}

    template <typename SetupF, typename F>
    static inline Val benchLatency(
        const size_t buckets_cnt, 
        const size_t bucket_iterations_cnt, 
        SetupF&& setup_func, F&& func
    );

    template <typename SetupF, typename F>
    static inline Val benchThroughput(
        const size_t buckets_cnt, 
        const size_t batches_cnt, 
        const size_t butch_iterations_cnt,
        SetupF&& setup_func, F&& func
    );

    // Если setup не указан, то функция будет вызываться 2 раза для прогрева.
    // Поэтому не получится корректно замерять производительность у функций с побоычным эффектов
    // по типу std::vector.push_back(val)
    template <typename F>
    static inline Val benchLatency(
        const size_t buckets_cnt, 
        const size_t bucket_iterations_cnt,
        F&& func
    );

private:

    class CycleTimer {

    public:

        static inline uint64_t start();
        static inline uint64_t end();
        
    };

};

//========================================IMPLEMETATION=============================================

template <typename SetupF, typename F>
Val Runner::benchThroughput(
    const size_t buckets_cnt, 
    const size_t batches_cnt, 
    const size_t batch_iterations_cnt,
    SetupF&& setup_func, F&& func
) {
    std::vector<double> buckets_mins(buckets_cnt);

    double mean = 0;

    using FReturnType = std::invoke_result_t<F, const size_t>;
    static constexpr bool func_is_void = std::is_void_v<FReturnType>;
    using SetupFReturnType = std::invoke_result_t<SetupF>;
    static constexpr bool setup_func_is_void = std::is_void_v<SetupFReturnType>;

    for (size_t bucket = 0; bucket < buckets_cnt; ++bucket) {

        double min = std::numeric_limits<double>::max();

        for (size_t batch = 0; batch < batches_cnt; ++batch) {

            if constexpr (setup_func_is_void) {
                setup_func();
            } 
            else {
                auto res = setup_func();
                DoNotOptimizeAway(res);
            }

            const uint64_t start = CycleTimer::start();
            for (size_t iteration = 0; iteration < batch_iterations_cnt; ++iteration) {

                if constexpr (func_is_void) {
                    func(iteration);
                } 
                else {
                    DoNotOptimizeAway(func(iteration));
                }
            }
            const uint64_t end = CycleTimer::end();

            const double time = static_cast<double>(end - start);
            const double avg_time = time / static_cast<double>(batch_iterations_cnt);

            if (avg_time < min) {
                min = avg_time;
            }
        }

        buckets_mins[bucket] = min;
        mean += min;
    }

    mean = mean / static_cast<double>(buckets_cnt);

    double disp = 0;

    for (size_t time_ind = 0; time_ind < buckets_cnt; ++time_ind) {
        const double x = buckets_mins[time_ind];
        const double dev = x - mean;
        disp += dev * dev;
    }

    assert(buckets_cnt > 1);

    disp = disp / static_cast<double>(buckets_cnt - 1);

    return {mean, std::sqrt(disp)};
}

template <typename SetupF, typename F>
Val Runner::benchLatency(
    const size_t buckets_cnt, 
    const size_t bucket_iterations_cnt, 
    SetupF&& setup_func,
    F&& func
) {
    std::vector<double> buckets_mins(buckets_cnt);

    double mean = 0;

    using FReturnType = std::invoke_result_t<F>;
    static constexpr bool func_is_void = std::is_void_v<FReturnType>;
    using SetupFReturnType = std::invoke_result_t<SetupF>;
    static constexpr bool setup_func_is_void = std::is_void_v<SetupFReturnType>;

    for (size_t bucket = 0; bucket < buckets_cnt; ++bucket) {

        double min = std::numeric_limits<double>::max();

        for (size_t iteration = 0; iteration < bucket_iterations_cnt; ++iteration) {

            if constexpr (setup_func_is_void) {
                setup_func();
            } 
            else {
                auto res = setup_func();
                DoNotOptimizeAway(res);
            }

            const uint64_t start = CycleTimer::start();

            if constexpr (func_is_void) {
                func();
            } 
            else {
                auto res = func();
                DoNotOptimizeAway(res);
            }

            const uint64_t end = CycleTimer::end();

            const double time = static_cast<double>(end - start);
            if (time < min) {
                min = time;
            }
        }

        buckets_mins[bucket] = min;
        mean += min;
    }

    mean = mean / static_cast<double>(buckets_cnt);

    double disp = 0;

    for (size_t time_ind = 0; time_ind < buckets_cnt; ++time_ind) {
        const double x = buckets_mins[time_ind];
        const double dev = x - mean;
        disp += dev * dev;
    }

    assert(buckets_cnt > 1);

    disp = disp / static_cast<double>(buckets_cnt - 1);

    return {mean, std::sqrt(disp)};
}


template <typename F>
Val Runner::benchLatency(
    const size_t buckets_cnt, 
    const size_t bucket_iterations_cnt,
    F&& func
) {
    return benchLatency(buckets_cnt, bucket_iterations_cnt, func, func);
}

uint64_t Runner::CycleTimer::start() {
    _mm_lfence();
    uint64_t clks = __rdtsc();
    _mm_lfence();
    return clks;
}

uint64_t Runner::CycleTimer::end() {
    unsigned int              DuMmMmMmMmMmY_________2286661337;
    uint64_t clks = __rdtscp(&DuMmMmMmMmMmY_________2286661337);
    _mm_lfence();
    return clks;
}


} // namespace measurer