#pragma once

#include "quant/binomial.hpp"
#include <cstdint>

namespace quant {

struct MonteCarloResult
{
    double price;
    double standard_error;
    double confidence_interval_lower;
    double confidence_interval_upper;
    std::uint64_t simulations;
};

MonteCarloResult monte_carlo_price(
    double spot,
    double strike,
    double rate,
    double volatility,
    double maturity,
    std::uint64_t simulations,
    OptionType option_type,
    std::uint64_t seed = 42,
    bool antithetic_variates = false
);

} // namespace quant