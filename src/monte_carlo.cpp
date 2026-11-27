#include "quant/monte_carlo.hpp"

#include <algorithm>
#include <cmath>
#include <random>
#include <stdexcept>

namespace quant {

MonteCarloResult monte_carlo_price(
    double spot,
    double strike,
    double rate,
    double volatility,
    double maturity,
    std::uint64_t simulations,
    OptionType option_type,
    std::uint64_t seed,
    bool antithetic_variates)
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
        throw std::invalid_argument(
            "Spot and strike prices must be positive.");

    if (volatility <= 0.0 || maturity <= 0.0)
        throw std::invalid_argument(
            "Volatility and maturity must be positive.");

    if (simulations < 2)
        throw std::invalid_argument(
            "At least two simulations are required.");

    if (option_type != OptionType::Call &&
        option_type != OptionType::Put)
    {
        throw std::invalid_argument("Invalid option type.");
    }

    if (antithetic_variates && simulations % 2 != 0)
    {
        throw std::invalid_argument(
            "Antithetic variates require an even simulation count.");
    }

    std::mt19937_64 generator(seed);
    std::normal_distribution<double> normal(0.0, 1.0);

    const double drift =
        (rate - 0.5 * volatility * volatility) * maturity;

    const double diffusion =
        volatility * std::sqrt(maturity);

    const double discount =
        std::exp(-rate * maturity);

    // Calculate one discounted option payoff.
    const auto discounted_payoff = [&](double z)
    {
        const double terminal_stock =
            spot * std::exp(drift + diffusion * z);

        if (!std::isfinite(terminal_stock))
        {
            throw std::overflow_error(
                "Non-finite simulated asset price.");
        }

        double payoff = 0.0;

        if (option_type == OptionType::Call)
            payoff = std::max(terminal_stock - strike, 0.0);
        else
            payoff = std::max(strike - terminal_stock, 0.0);

        const double result = discount * payoff;

        if (!std::isfinite(result))
        {
            throw std::overflow_error(
                "Non-finite discounted option payoff.");
        }

        return result;
    };

    double mean = 0.0;
    double m2 = 0.0;

    // Number of statistically independent observations.
    const std::uint64_t observations =
        antithetic_variates ? simulations / 2 : simulations;

    for (std::uint64_t i = 1; i <= observations; ++i)
    {
        const double z = normal(generator);

        double observation = 0.0;

        if (antithetic_variates)
        {
            const double payoff_positive = discounted_payoff(z);
            const double payoff_negative = discounted_payoff(-z);

            observation =
                0.5 * (payoff_positive + payoff_negative);
        }
        else
        {
            observation = discounted_payoff(z);
        }

        // Welford's online mean and variance.
        const double delta = observation - mean;

        mean += delta / static_cast<double>(i);

        const double delta2 = observation - mean;

        m2 += delta * delta2;
    }

    const double sample_variance =
        m2 / static_cast<double>(observations - 1);

    const double standard_error =
        std::sqrt(
            sample_variance / static_cast<double>(observations));

    constexpr double CONFIDENCE_FACTOR = 1.96;

    return {
        mean,
        standard_error,
        mean - CONFIDENCE_FACTOR * standard_error,
        mean + CONFIDENCE_FACTOR * standard_error,
        simulations
    };
}

} // namespace quant