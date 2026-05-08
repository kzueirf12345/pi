#include "Testing/Testing.hpp"

#include <iomanip>
#include <ostream>

namespace pi {

using namespace mpfr;

static size_t digitsToBits(size_t digits) {
    return static_cast<size_t>(digits * 3.32192809488736234787) + 16;
}

void VerifyAlgorithm(
    std::ostream& out, const std::string& algo_name, PiAlgoFunc algo, size_t decimal_digits
) {
    out << "\n[Verification] " << algo_name << " (" << decimal_digits << " digits)\n";
    out << std::string(60, '-') << "\n";

    const size_t prec_bits = digitsToBits(decimal_digits);
    mpreal::set_default_prec(prec_bits);

    mpreal reference_pi = mpfr::const_pi();

    mpreal calculated_pi = algo(decimal_digits);

    std::ostringstream ref_ss, calc_ss;
    ref_ss << std::fixed << std::setprecision(decimal_digits) << reference_pi;
    calc_ss << std::fixed << std::setprecision(decimal_digits) << calculated_pi;

    std::string ref_str = ref_ss.str();
    std::string calc_str = calc_ss.str();

    
    size_t matched = 0;
    for (size_t i = 0; i < ref_str.size() && i < calc_str.size(); ++i) {
        if (ref_str[i] == calc_str[i]) {
            matched++;
        } else {
            break;
        }
    }

    size_t matched_digits = matched > 1 ? matched - 2 : 0;

    out << "Reference:  " << ref_str.substr(0, 50) << (ref_str.size() > 50 ? "..." : "") << "\n";
    out << "Calculated: " << calc_str.substr(0, 50) << (calc_str.size() > 50 ? "..." : "") << "\n";
    
    if (matched_digits >= decimal_digits) {
        out << "Status:     [SUCCESS] All requested digits match!\n";
    } else {
        out << "Status:     [FAILED]  Matched only " << matched_digits 
            << " out of " << decimal_digits << " digits.\n";
    }
}

} //namespace pi