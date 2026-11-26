#include "quant/greeks.hpp"

#include <cmath>
#include <stdexcept>

namespace quant {

namespace {

constexpr double PI = 3.14159265358979323846;

double normal_cdf(double x)
{
    return 0.5 * std::erfc(-x / std::sqrt(2.0));
}

double normal_pdf(double x)
{
    return std::exp(-0.5 * x * x) / std::sqrt(2.0 * PI);
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

struct Parameters {
    double d1;
    double d2;
};

Parameters calculate_d1_d2(
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


OptionGreeks black_scholes_call_greeks(
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

    const double pdf_d1 = normal_pdf(d1);
    const double discount = std::exp(-rate * maturity);
    const double sqrt_t = std::sqrt(maturity);

    const double delta = normal_cdf(d1);

    const double gamma =
        pdf_d1 / (spot * volatility * sqrt_t);

    const double vega =
        spot * pdf_d1 * sqrt_t;

    const double theta =
        -spot * pdf_d1 * volatility / (2.0 * sqrt_t)
        - rate * strike * discount * normal_cdf(d2);

    const double rho =
        strike * maturity * discount * normal_cdf(d2);

    return {delta, gamma, vega, theta, rho};
}


OptionGreeks black_scholes_put_greeks(
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

    const double pdf_d1 = normal_pdf(d1);
    const double discount = std::exp(-rate * maturity);
    const double sqrt_t = std::sqrt(maturity);

    const double delta = normal_cdf(d1) - 1.0;

    const double gamma =
        pdf_d1 / (spot * volatility * sqrt_t);

    const double vega =
        spot * pdf_d1 * sqrt_t;

    const double theta =
        -spot * pdf_d1 * volatility / (2.0 * sqrt_t)
        + rate * strike * discount * normal_cdf(-d2);

    const double rho =
        -strike * maturity * discount * normal_cdf(-d2);

    return {delta, gamma, vega, theta, rho};
}

}