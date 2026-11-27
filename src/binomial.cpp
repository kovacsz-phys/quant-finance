#include "quant/binomial.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>

namespace quant {

double crr_binomial_price(
    double spot,
    double strike,
    double rate,
    double volatility,
    double maturity,
    int steps,
    OptionType option_type,
    ExerciseStyle exercise_style)
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

    const double dt = maturity / steps;
    const double sqrt_dt = std::sqrt(dt);

    // Up and down factors.
    const double u = std::exp(volatility * sqrt_dt);
    const double d = 1.0 / u;

    // Risk-neutral probability.
    const double growth = std::exp(rate * dt);

    const double p = (growth - d) / (u - d);

    if (!std::isfinite(p) || p < 0.0 || p > 1.0)
    {
        throw std::invalid_argument(
            "Invalid risk-neutral probability. "
            "Try increasing the number of steps.");
    }

    const double discount = std::exp(-rate * dt);

   // Option values at maturity.
    std::vector<double> values(steps + 1);

    // Calculate the first terminal stock price.
    double stock_price =
        spot * std::exp(-steps * volatility * sqrt_dt);

    // Calculate the multiplicative up/down ratio.
    const double price_ratio =
        std::exp(2.0 * volatility * sqrt_dt);

    // Generate subsequent stock prices using multiplication.
    for (int j = 0; j <= steps; ++j)
    {
        if (option_type == OptionType::Call)
        {
            values[j] = std::max(stock_price - strike, 0.0);
        }
        else
        {
            values[j] = std::max(strike - stock_price, 0.0);
        }

        stock_price *= price_ratio;
    }

    // Backward induction.
    for (int i = steps - 1; i >= 0; --i)
    {
        for (int j = 0; j <= i; ++j)
        {
            const double continuation_value =
                discount *
                (p * values[j + 1]
                 + (1.0 - p) * values[j]);

            if (exercise_style == ExerciseStyle::American)
            {
                const double exponent =
                    (2.0 * j - i) * volatility * sqrt_dt;

                const double stock_price =
                    spot * std::exp(exponent);

                double exercise_value;

                if (option_type == OptionType::Call)
                {
                    exercise_value =
                        std::max(stock_price - strike, 0.0);
                }
                else
                {
                    exercise_value =
                        std::max(strike - stock_price, 0.0);
                }

                values[j] =
                    std::max(continuation_value, exercise_value);
            }
            else
            {
                values[j] = continuation_value;
            }
        }
    }

    return values[0];
}

}