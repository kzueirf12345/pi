#include <stdexcept>

#include "CliParser/CliParser.hpp"

CliParser::CliParser(int argc, char* argv[]) {
    parse(argc, argv);
}

void CliParser::printHelp(std::ostream& output) {
    output << "Usage: ./build/measurer [OPTIONS]\n"
              "Options:\n"
              "  -m, --mode <MODE_NAME>     Specify execution mode | "
              
#define DEFINE_ENUM_(name) << #name << " |"
        MODE_LIST_(DEFINE_ENUM_)
#undef DEFINE_ENUM_
              
           << " (default: LATENCY)\n"
              "  -o, --output <FILE>        Specify output file (default: stdout)\n"
              "  -u, --buckets <VALUE>      Specify buckets count (default: 10)\n"
              "  -a, --batches <VALUE>      Specify batches count (default: 50)\n"
              "  -n, --iterations <VALUE>   Specify iterations count in bucket or in batch (default: 50000)\n"
              "  -s, --seed <VALUE>         Specify seed for random (default: random)\n"
              "  -v, --verbose              Output exectuion progress\n"
              "  -h, --help                 Show this help message\n";
}

void CliParser::parse(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            options_.help_requested = true;
            return;
        }
        else if (arg == "-o" || arg == "--output") {
            checkRequireArgument(i, argc, arg);
            options_.output_file = argv[++i];
        }
        else if (arg == "-u" || arg == "--buckets") {
            checkRequireArgument(i, argc, arg);
            options_.buckets_cnt = parseSize(argv[++i], "--buckets");
        }
        else if (arg == "-a" || arg == "--batches") {
            checkRequireArgument(i, argc, arg);
            options_.batches_cnt = parseSize(argv[++i], "--batches");
        }
        else if (arg == "-m" || arg == "--mode") {
            checkRequireArgument(i, argc, arg);
            options_.mode = parseMode(argv[++i], "--mode");
        }
        else if (arg == "-n" || arg == "--iterations") {
            checkRequireArgument(i, argc, arg);
            options_.iterations_cnt = parseSize(argv[++i], "--iterations");
        }
        else if (arg == "-s" || arg == "--seed") {
            checkRequireArgument(i, argc, arg);
            options_.seed = parseUint64(argv[++i], "--seed");
        }
        else if (arg == "-v" || arg == "--verbose") {
            options_.verbose = true;
        }
        else if (arg.starts_with('-')) {
            throw std::invalid_argument("Unknown option: " + arg);
        }
        else {
            throw std::invalid_argument("Unexpected positional argument: " + arg);
        }
    }
}

void CliParser::checkRequireArgument(int index, int argc, const std::string& option) {
    if (index + 1 >= argc) {
        throw std::invalid_argument("Option " + option + " requires an argument");
    }
}

uint64_t CliParser::parseUint64(const char* str, const std::string& option) try {
    size_t pos = 0;

    unsigned long val = std::stoul(str, &pos);
    if (pos != std::strlen(str)) {
        throw std::invalid_argument("");
    }

    return static_cast<uint64_t>(val);
}
catch (...) {
    throw std::invalid_argument("Invalid uint64_t value for " + option + ": " + str);
}

size_t CliParser::parseSize(const char* str, const std::string& option) try {
    size_t pos = 0;

    unsigned long val = std::stoul(str, &pos);
    if (pos != std::strlen(str)) {
        throw std::invalid_argument("");
    }

    return static_cast<size_t>(val);
}
catch (...) {
    throw std::invalid_argument("Invalid size_t value for " + option + ": " + str);
}

CliParser::Mode CliParser::parseMode(const char* str, const std::string& option) try {
    return mode_str2enum_map.at(str);
}
catch (...) {
    throw std::invalid_argument("Invalid mode for " + option + ": " + str);
}