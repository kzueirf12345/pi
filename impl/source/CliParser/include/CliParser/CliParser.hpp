#pragma once

#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <random>
#include <string>
#include <string_view>
#include <unordered_map>

class CliParser {

public:

#define MODE_LIST_(V) \
    V(BENCH_MONTE_CARLO) \
    V(BENCH_GAUSS_LEGENRE_AND_CHUDNOVSKY)

#define DEFINE_ENUM_(name) name,
    enum class Mode {
        MODE_LIST_(DEFINE_ENUM_)
    };
#undef DEFINE_ENUM_

#define DEFINE_ENUM_(name) {Mode::name, #name},
    static inline const std::unordered_map<enum Mode, std::string_view> mode_enum2str_map {
        MODE_LIST_(DEFINE_ENUM_)
    };
#undef DEFINE_ENUM_

#define DEFINE_ENUM_(name) {#name, Mode::name},
    static inline const std::unordered_map<std::string_view, enum Mode> mode_str2enum_map {
        MODE_LIST_(DEFINE_ENUM_)
    };
#undef DEFINE_ENUM_

public:

    struct Options {
        std::string output_file = "";
        size_t buckets_cnt = 10;
        size_t batches_cnt = 50;
        size_t iterations_cnt = 50000;
        Mode mode = Mode::BENCH_MONTE_CARLO;
        uint64_t seed = std::random_device{}();
        bool verbose = false;
        bool help_requested = false;
    };

public:

    CliParser(int argc, char* argv[]);

    [[nodiscard]] inline const Options& options    () const noexcept { return options_; }
    [[nodiscard]] inline bool           shouldExit () const noexcept { return options_.help_requested; }

    static void printHelp(std::ostream& output);

private:

    void parse(int argc, char* argv[]);

    static void checkRequireArgument(int index, int argc, const std::string& option);

    static uint64_t parseUint64(const char* str, const std::string& option);
    static size_t parseSize(const char* str, const std::string& option);
    static Mode parseMode(const char* str, const std::string& option);

private:

    Options options_;

};