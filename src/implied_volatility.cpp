#include "quant/implied_volatility.hpp"

#include "quant/black_scholes.hpp"
#include "quant/greeks.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace quant {

namespace {

constexpr double MIN_VOLATILITY = 1e-8;
constexpr double MAX_VOLATILITY = 1e4;

constexpr double PRICE_TOLERANCE = 1e-10;
constexpr double VOLATILITY_TOLERANCE = 1e-10;

constexpr int MAX_ITERATIONS = 200;

double solve_implied_volatility(
    double market_price,
    double spot,
    double strike,
    double rate,
    double maturity,
    double initial_volatility,
    bool is_call)
{
    // Validate inputs.
    if (!std::isfinite(market_price) ||
        !std::isfinite(spot) ||
        !std::isfinite(strike) ||
        !std::isfinite(rate) ||
        !std::isfinite(maturity) ||
        !std::isfinite(initial_volatility))
    {
        throw std::invalid_argument(
            "All input parameters must be finite.");
    }

    if (market_price <= 0.0 ||
        spot <= 0.0 ||
        strike <= 0.0 ||
        maturity <= 0.0 ||
        initial_volatility <= 0.0)
    {
        throw std::invalid_argument(
            "Prices, maturity, and initial volatility must be positive.");
    }

    // Calculate theoretical lower and upper price bounds.
    const double discounted_strike =
        strike * std::exp(-rate * maturity);

    const double lower_bound = is_call
        ? std::max(spot - discounted_strike, 0.0)
        : std::max(discounted_strike - spot, 0.0);

    const double upper_bound = is_call
        ? spot
        : discounted_strike;

    if (market_price <= lower_bound ||
        market_price >= upper_bound)
    {
        throw std::invalid_argument(
            "Market price is outside the range for a finite implied volatility.");
    }

    // Pricing function.
    const auto price = [&](double volatility)
    {
        return is_call
            ? black_scholes_call(
                spot, strike, rate, volatility, maturity)
            : black_scholes_put(
                spot, strike, rate, volatility, maturity);
    };

    // Find an initial volatility bracket.
    double low = MIN_VOLATILITY;
    double high = 1.0;

    while (price(high) < market_price &&
           high < MAX_VOLATILITY)
    {
        high *= 2.0;
    }

    if (price(high) < market_price)
    {
        throw std::runtime_error(
            "Unable to bracket implied volatility.");
    }

    // Start Newton-Raphson inside the bracket.
    double volatility = std::clamp(
        initial_volatility, low, high);

    for (int iteration = 0;
         iteration < MAX_ITERATIONS;
         ++iteration)
    {
        const double model_price = price(volatility);
        const double error = model_price - market_price;

        if (std::abs(error) < PRICE_TOLERANCE)
        {
            return volatility;
        }

        // Update the volatility bracket.
        if (error > 0.0)
        {
            high = volatility;
        }
        else
        {
            low = volatility;
        }

        if (high - low < VOLATILITY_TOLERANCE)
        {
            return 0.5 * (low + high);
        }

        // Calculate Vega.
        const OptionGreeks greeks = is_call
            ? black_scholes_call_greeks(
                spot, strike, rate, volatility, maturity)
            : black_scholes_put_greeks(
                spot, strike, rate, volatility, maturity);

        // Newton-Raphson candidate.
        double candidate = 0.5 * (low + high);

        if (greeks.vega > 1e-12)
        {
            const double newton_candidate =
                volatility - error / greeks.vega;

            // Accept Newton step only if it remains inside
            // the current bracket.
            if (newton_candidate > low &&
                newton_candidate < high &&
                std::isfinite(newton_candidate))
            {
                candidate = newton_candidate;
            }
        }

        // Otherwise, use bisection.
        volatility = candidate;
    }

    throw std::runtime_error(
        "Implied volatility solver did not converge.");
}

} // anonymous namespace


double implied_volatility_call(
    double market_price,
    double spot,
    double strike,
    double rate,
    double maturity,
    double initial_volatility)
{
    return solve_implied_volatility(
        market_price,
        spot,
        strike,
        rate,
        maturity,
        initial_volatility,
        true);
}


double implied_volatility_put(
    double market_price,
    double spot,
    double strike,
    double rate,
    double maturity,
    double initial_volatility)
{
    return solve_implied_volatility(
        market_price,
        spot,
        strike,
        rate,
        maturity,
        initial_volatility,
        false);
}

}