#include "quant/binomial.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace quant {

double crr_european_price_direct(
    double spot,
    double strike,
    double rate,
    double volatility,
    double maturity,
    int steps,
    OptionType option_type)
{
    // Validate inputs.
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

    if (steps <= 0)
    {
        throw std::invalid_argument(
            "Number of steps must be positive.");
    }

    if (option_type != OptionType::Call &&
        option_type != OptionType::Put)
    {
        throw std::invalid_argument("Invalid option type.");
    }

    const double dt = maturity / steps;
    const double sqrt_dt = std::sqrt(dt);

    const double u = std::exp(volatility * sqrt_dt);
    const double d = 1.0 / u;

    const double growth = std::exp(rate * dt);
    const double p = (growth - d) / (u - d);

    if (!std::isfinite(p) || p < 0.0 || p > 1.0)
    {
        throw std::invalid_argument(
            "Invalid risk-neutral probability. "
            "Try increasing the number of steps.");
    }

    const double discount = std::exp(-rate * maturity);
    const double price_ratio =
        std::exp(2.0 * volatility * sqrt_dt);

    // Degenerate binomial distributions.
    if (p == 0.0 || p == 1.0)
    {
        const double terminal_stock =
            p == 0.0
                ? spot * std::pow(d, steps)
                : spot * std::pow(u, steps);

        const double payoff =
            option_type == OptionType::Call
                ? std::max(terminal_stock - strike, 0.0)
                : std::max(strike - terminal_stock, 0.0);

        return discount * payoff;
    }

    // Find the mode of the binomial distribution.
    const int mode = static_cast<int>(
        std::floor((steps + 1.0) * p));

    // Calculate the probability at the mode using logarithms.
    const double log_probability =
        std::lgamma(steps + 1.0)
        - std::lgamma(mode + 1.0)
        - std::lgamma(steps - mode + 1.0)
        + mode * std::log(p)
        + (steps - mode) * std::log1p(-p);

    const double mode_probability =
        std::exp(log_probability);

    // Stock price corresponding to the mode.
    double stock_at_mode =
        spot * std::exp(
            (2.0 * mode - steps) * volatility * sqrt_dt);

    // Compensated summation.
    double sum = 0.0;
    double compensation = 0.0;

    auto add_payoff = [&](double probability, double stock_price)
    {
        const double payoff =
            option_type == OptionType::Call
                ? std::max(stock_price - strike, 0.0)
                : std::max(strike - stock_price, 0.0);

        const double term = probability * payoff;

        const double adjusted_term = term - compensation;
        const double temporary_sum = sum + adjusted_term;

        compensation =
            (temporary_sum - sum) - adjusted_term;

        sum = temporary_sum;
    };

    // Add the mode.
    add_payoff(mode_probability, stock_at_mode);

    // Sum probabilities above the mode.
    double probability = mode_probability;
    double stock_price = stock_at_mode;

    for (int j = mode; j < steps; ++j)
    {
        probability *=
            (static_cast<double>(steps - j) / (j + 1))
            * (p / (1.0 - p));

        stock_price *= price_ratio;

        add_payoff(probability, stock_price);
    }

    // Sum probabilities below the mode.
    probability = mode_probability;
    stock_price = stock_at_mode;

    for (int j = mode; j > 0; --j)
    {
        probability *=
            (static_cast<double>(j) / (steps - j + 1))
            * ((1.0 - p) / p);

        stock_price /= price_ratio;

        add_payoff(probability, stock_price);
    }

    return discount * sum;
}

} // namespace quant