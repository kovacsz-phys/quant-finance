#include "quant/black_scholes.hpp"

#include <cmath>
#include <stdexcept>

namespace quant {

namespace {

double normal_cdf(double x)
{
    return 0.5 * std::erfc(-x / std::sqrt(2.0));
}

void validate_inputs(
    double spot,
    double strike,
    double rate,
    double volatility,
    double maturity)
{
    if (!std::isfinite(spot) ||
        !std::isfinite(strike) ||
        !std::isfinite(rate) ||
        !std::isfinite(volatility) ||
        !std::isfinite(maturity))
    {
        throw std::invalid_argument(
            "All input parameters must be finite.");
    }

    if (spot <= 0.0 || strike <= 0.0)
    {
        throw std::invalid_argument(
            "Spot and strike prices must be positive.");
    }

    if (volatility <= 0.0 || maturity <= 0.0)
    {
        throw std::invalid_argument(
            "Volatility and maturity must be positive.");
    }
}

struct BlackScholesParameters {
    double d1;
    double d2;
};

BlackScholesParameters calculate_d1_d2(
    double spot,
    double strike,
    double rate,
    double volatility,
    double maturity)
{
    const double sqrt_t = std::sqrt(maturity);

    const double d1 =
        (std::log(spot / strike)
         + (rate + 0.5 * volatility * volatility) * maturity)
        / (volatility * sqrt_t);

    const double d2 = d1 - volatility * sqrt_t;

    return {d1, d2};
}

} // anonymous namespace

double black_scholes_call(
    double spot,
    double strike,
    double rate,
    double volatility,
    double maturity)
{
    validate_inputs(
        spot, strike, rate, volatility, maturity);

    const auto [d1, d2] = calculate_d1_d2(
        spot, strike, rate, volatility, maturity);

    return spot * normal_cdf(d1)
        - strike * std::exp(-rate * maturity)
        * normal_cdf(d2);
}

double black_scholes_put(
    double spot,
    double strike,
    double rate,
    double volatility,
    double maturity)
{
    validate_inputs(
        spot, strike, rate, volatility, maturity);

    const auto [d1, d2] = calculate_d1_d2(
        spot, strike, rate, volatility, maturity);

    return strike * std::exp(-rate * maturity)
        * normal_cdf(-d2)
        - spot * normal_cdf(-d1);
}

}