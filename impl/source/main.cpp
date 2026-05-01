#include <climits>
#include <cstdlib>
#include <fstream>
#include <stdexcept>

#include "CliParser/CliParser.hpp"
#include "Measurer/Measurer.hpp"

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

    switch (opts.mode) {
        case CliParser::Mode::MAIN: {
            *output_ptr << "Hello, World!\n";
            break;
        }

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