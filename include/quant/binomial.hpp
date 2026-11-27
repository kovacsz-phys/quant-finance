#pragma once

namespace quant {

enum class OptionType {
    Call,
    Put
};

enum class ExerciseStyle {
    European,
    American
};

double crr_binomial_price(
    double spot,
    double strike,
    double rate,
    double volatility,
    double maturity,
    int steps,
    OptionType option_type,
    ExerciseStyle exercise_style
);

double crr_european_price_direct(
    double spot,
    double strike,
    double rate,
    double volatility,
    double maturity,
    int steps,
    OptionType option_type
);

}
