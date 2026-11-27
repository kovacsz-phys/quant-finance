#include "quant/monte_carlo.hpp"
#include "quant/black_scholes.hpp"

#include <cmath>
#include <limits>
#include <stdexcept>

#include <gtest/gtest.h>

namespace {

constexpr double SPOT = 100.0;
constexpr double STRIKE = 100.0;
constexpr double RATE = 0.05;
constexpr double VOLATILITY = 0.20;
constexpr double MATURITY = 1.0;

constexpr std::uint64_t SIMULATIONS = 1'000'000;
constexpr std::uint64_t SEED = 42;

TEST(MonteCarloTest, EuropeanCallPrice)
{
    const auto result = quant::monte_carlo_price(
        SPOT, STRIKE, RATE, VOLATILITY, MATURITY,
        SIMULATIONS, quant::OptionType::Call, SEED);

    const double analytical_price =
        quant::black_scholes_call(
            SPOT, STRIKE, RATE, VOLATILITY, MATURITY);

    EXPECT_NEAR(result.price, analytical_price, 0.05);
}

TEST(MonteCarloTest, EuropeanPutPrice)
{
    const auto result = quant::monte_carlo_price(
        SPOT, STRIKE, RATE, VOLATILITY, MATURITY,
        SIMULATIONS, quant::OptionType::Put, SEED);

    const double analytical_price =
        quant::black_scholes_put(
            SPOT, STRIKE, RATE, VOLATILITY, MATURITY);

    EXPECT_NEAR(result.price, analytical_price, 0.05);
}

TEST(MonteCarloTest, ReproducibleWithSameSeed)
{
    const auto first = quant::monte_carlo_price(
        SPOT, STRIKE, RATE, VOLATILITY, MATURITY,
        10000, quant::OptionType::Call, SEED);

    const auto second = quant::monte_carlo_price(
        SPOT, STRIKE, RATE, VOLATILITY, MATURITY,
        10000, quant::OptionType::Call, SEED);

    EXPECT_DOUBLE_EQ(first.price, second.price);
    EXPECT_DOUBLE_EQ(first.standard_error, second.standard_error);
}

TEST(MonteCarloTest, DifferentSeedsProduceDifferentEstimates)
{
    const auto first = quant::monte_carlo_price(
        SPOT, STRIKE, RATE, VOLATILITY, MATURITY,
        10000, quant::OptionType::Call, 42);

    const auto second = quant::monte_carlo_price(
        SPOT, STRIKE, RATE, VOLATILITY, MATURITY,
        10000, quant::OptionType::Call, 123);

    EXPECT_NE(first.price, second.price);
}

TEST(MonteCarloTest, StandardErrorIsPositive)
{
    const auto result = quant::monte_carlo_price(
        SPOT, STRIKE, RATE, VOLATILITY, MATURITY,
        SIMULATIONS, quant::OptionType::Call, SEED);

    EXPECT_GT(result.standard_error, 0.0);
}

TEST(MonteCarloTest, ConfidenceIntervalContainsEstimate)
{
    const auto result = quant::monte_carlo_price(
        SPOT, STRIKE, RATE, VOLATILITY, MATURITY,
        SIMULATIONS, quant::OptionType::Call, SEED);

    EXPECT_LE(result.confidence_interval_lower, result.price);
    EXPECT_GE(result.confidence_interval_upper, result.price);
}

TEST(MonteCarloTest, SimulationCountIsReturned)
{
    const auto result = quant::monte_carlo_price(
        SPOT, STRIKE, RATE, VOLATILITY, MATURITY,
        10000, quant::OptionType::Call, SEED);

    EXPECT_EQ(result.simulations, 10000);
}

TEST(MonteCarloTest, RejectsInvalidSimulationCount)
{
    EXPECT_THROW(
        quant::monte_carlo_price(
            SPOT, STRIKE, RATE, VOLATILITY, MATURITY,
            1, quant::OptionType::Call),
        std::invalid_argument);
}

TEST(MonteCarloTest, RejectsNegativeVolatility)
{
    EXPECT_THROW(
        quant::monte_carlo_price(
            SPOT, STRIKE, RATE, -0.2, MATURITY,
            SIMULATIONS, quant::OptionType::Call),
        std::invalid_argument);
}

TEST(MonteCarloTest, RejectsInvalidSpot)
{
    EXPECT_THROW(
        quant::monte_carlo_price(
            0.0, STRIKE, RATE, VOLATILITY, MATURITY,
            SIMULATIONS, quant::OptionType::Call),
        std::invalid_argument);
}

TEST(MonteCarloTest, RejectsNonFiniteInput)
{
    EXPECT_THROW(
        quant::monte_carlo_price(
            SPOT, STRIKE, RATE,
            std::numeric_limits<double>::infinity(),
            MATURITY, SIMULATIONS,
            quant::OptionType::Call),
        std::invalid_argument);
}

TEST(MonteCarloTest, AntitheticCallPriceMatchesBlackScholes)
{
    constexpr double spot = 100.0;
    constexpr double strike = 100.0;
    constexpr double rate = 0.05;
    constexpr double volatility = 0.20;
    constexpr double maturity = 1.0;

    const auto result = quant::monte_carlo_price(
        spot, strike, rate, volatility, maturity,
        1000000, quant::OptionType::Call, 42, true);

    const double reference = quant::black_scholes_call(
        spot, strike, rate, volatility, maturity);

    EXPECT_NEAR(result.price, reference, 0.05);
    EXPECT_EQ(result.simulations, 1000000);
}

TEST(MonteCarloTest, AntitheticPutPriceMatchesBlackScholes)
{
    constexpr double spot = 100.0;
    constexpr double strike = 100.0;
    constexpr double rate = 0.05;
    constexpr double volatility = 0.20;
    constexpr double maturity = 1.0;

    const auto result = quant::monte_carlo_price(
        spot, strike, rate, volatility, maturity,
        1000000, quant::OptionType::Put, 42, true);

    const double reference = quant::black_scholes_put(
        spot, strike, rate, volatility, maturity);

    EXPECT_NEAR(result.price, reference, 0.05);
    EXPECT_EQ(result.simulations, 1000000);
}

TEST(MonteCarloTest, AntitheticVariatesReduceStandardErrorForCall)
{
    const auto standard = quant::monte_carlo_price(
        100.0, 100.0, 0.05, 0.20, 1.0,
        100000, quant::OptionType::Call, 42);

    const auto antithetic = quant::monte_carlo_price(
        100.0, 100.0, 0.05, 0.20, 1.0,
        100000, quant::OptionType::Call, 42, true);

    EXPECT_LT(
        antithetic.standard_error,
        standard.standard_error);
}

TEST(MonteCarloTest, AntitheticVariatesReduceStandardErrorForPut)
{
    const auto standard = quant::monte_carlo_price(
        100.0, 100.0, 0.05, 0.20, 1.0,
        100000, quant::OptionType::Put, 42);

    const auto antithetic = quant::monte_carlo_price(
        100.0, 100.0, 0.05, 0.20, 1.0,
        100000, quant::OptionType::Put, 42, true);

    EXPECT_LT(
        antithetic.standard_error,
        standard.standard_error);
}

TEST(MonteCarloTest, AntitheticVariatesAreReproducible)
{
    const auto first = quant::monte_carlo_price(
        100.0, 100.0, 0.05, 0.20, 1.0,
        100000, quant::OptionType::Call, 42, true);

    const auto second = quant::monte_carlo_price(
        100.0, 100.0, 0.05, 0.20, 1.0,
        100000, quant::OptionType::Call, 42, true);

    EXPECT_DOUBLE_EQ(first.price, second.price);
    EXPECT_DOUBLE_EQ(
        first.standard_error,
        second.standard_error);
}

TEST(MonteCarloTest, AntitheticVariatesRejectOddSimulationCount)
{
    EXPECT_THROW(
        quant::monte_carlo_price(
            100.0, 100.0, 0.05, 0.20, 1.0,
            999, quant::OptionType::Call, 42, true),
        std::invalid_argument);
}

} // namespace