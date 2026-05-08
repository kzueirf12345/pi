#include <climits>
#include <cstdlib>
#include <fstream>
#include <stdexcept>
#include <unistd.h>

#include "Calculation/Chudnovsky.hpp"
#include "Calculation/GaussLegendre.hpp"
#include "CliParser/CliParser.hpp"
#include "Calculation/MonteCarlo.hpp"
#include "Testing/Testing.hpp"

int main(int argc, char* argv[]) try {
    CliParser cli(argc, argv);
    
    if (cli.shouldExit()) {
        CliParser::printHelp(std::cout);
        return EXIT_SUCCESS;
    }

    const CliParser::Options& opts = cli.options();

    std::ostream* output_ptr = &std::cout;
    std::ofstream output_file;
    
    if (!opts.output_file.empty()) {
        output_file.open(opts.output_file);
        if (!output_file.is_open()) {
            throw std::invalid_argument("Can't open output file: " + opts.output_file);
        }
        output_ptr = &output_file;
    }

    (void)output_ptr;

    switch (opts.mode) {
        case CliParser::Mode::BENCH_MONTE_CARLO: {
            pi::BenchMonteCarlo(
                *output_ptr, opts.seed, opts.buckets_cnt, opts.batches_cnt, opts.iterations_cnt
            );
        } break;

        case CliParser::Mode::TESTING_GAUSS_LEGENRE_AND_CHUDNOVSKY: {
            pi::VerifyAlgorithm(*output_ptr, "Gauss-Legendre Dummy", pi::GaussLegendre, opts.iterations_cnt);
            pi::VerifyAlgorithm(*output_ptr, "Chudnovsky Dummy", pi::Chudnovsky, opts.iterations_cnt);
        } break;

        case CliParser::Mode::BENCH_GAUSS_LEGENRE_AND_CHUDNOVSKY: {
            pi::BenchAlgorithm(*output_ptr, "Gauss-Legendre", pi::GaussLegendre, opts.iterations_cnt, opts.buckets_cnt, opts.batches_cnt);
            pi::BenchAlgorithm(*output_ptr, "Chudnovsky",     pi::Chudnovsky,    opts.iterations_cnt, opts.buckets_cnt, opts.batches_cnt);
        } break;

        default: {
            std::cerr << "Unhandled mode!" << std::endl;
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
catch(const std::exception& e) {
    std::cerr << "!!!EXCEPTION!!!\n" << e.what() << std::endl;
    return EXIT_FAILURE;
}
catch(...) {
    std::cerr << "!!!EXCEPTION!!!\n" << "Something went wrong!" << std::endl;
    return EXIT_FAILURE;
}