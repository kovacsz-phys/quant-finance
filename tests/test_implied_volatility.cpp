#include "quant/black_scholes.hpp"
#include "quant/implied_volatility.hpp"

#include <gtest/gtest.h>
#include <stdexcept>

namespace {

constexpr double S = 100.0;
constexpr double K = 100.0;
constexpr double r = 0.05;
constexpr double T = 1.0;
constexpr double TRUE_VOLATILITY = 0.20;

} // anonymous namespace


TEST(ImpliedVolatilityTest, RecoversCallVolatility)
{
    const double market_price =
        quant::black_scholes_call(
            S, K, r, TRUE_VOLATILITY, T);

    const double implied_volatility =
        quant::implied_volatility_call(
            market_price, S, K, r, T);

    EXPECT_NEAR(
        implied_volatility,
        TRUE_VOLATILITY,
        1e-8);
}


TEST(ImpliedVolatilityTest, RecoversPutVolatility)
{
    const double market_price =
        quant::black_scholes_put(
            S, K, r, TRUE_VOLATILITY, T);

    const double implied_volatility =
        quant::implied_volatility_put(
            market_price, S, K, r, T);

    EXPECT_NEAR(
        implied_volatility,
        TRUE_VOLATILITY,
        1e-8);
}


TEST(ImpliedVolatilityTest, WorksWithDifferentInitialGuess)
{
    const double market_price =
        quant::black_scholes_call(
            S, K, r, TRUE_VOLATILITY, T);

    const double implied_volatility =
        quant::implied_volatility_call(
            market_price, S, K, r, T, 1.5);

    EXPECT_NEAR(
        implied_volatility,
        TRUE_VOLATILITY,
        1e-8);
}


TEST(ImpliedVolatilityTest, RejectsInvalidMarketPrice)
{
    EXPECT_THROW(
        quant::implied_volatility_call(
            150.0, S, K, r, T),
        std::invalid_argument);
}


TEST(ImpliedVolatilityTest, RejectsZeroMarketPrice)
{
    EXPECT_THROW(
        quant::implied_volatility_put(
            0.0, S, K, r, T),
        std::invalid_argument);
}