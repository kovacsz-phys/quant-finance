#include "quant/black_scholes.hpp"

#include <gtest/gtest.h>
#include <cmath>
#include <stdexcept>

TEST(BlackScholesTest, EuropeanCallPrice)
{
    const double price = quant::black_scholes_call(
        100.0, 100.0, 0.05, 0.20, 1.0);

    EXPECT_NEAR(price, 10.450584, 1e-5);
}

TEST(BlackScholesTest, EuropeanPutPrice)
{
    const double price = quant::black_scholes_put(
        100.0, 100.0, 0.05, 0.20, 1.0);

    EXPECT_NEAR(price, 5.573526, 1e-5);
}

TEST(BlackScholesTest, PutCallParity)
{
    const double spot = 100.0;
    const double strike = 100.0;
    const double rate = 0.05;
    const double volatility = 0.20;
    const double maturity = 1.0;

    const double call = quant::black_scholes_call(
        spot, strike, rate, volatility, maturity);

    const double put = quant::black_scholes_put(
        spot, strike, rate, volatility, maturity);

    const double parity =
        spot - strike * std::exp(-rate * maturity);

    EXPECT_NEAR(call - put, parity, 1e-10);
}

TEST(BlackScholesTest, RejectsInvalidSpotPrice)
{
    EXPECT_THROW(
        quant::black_scholes_call(
            -100.0, 100.0, 0.05, 0.20, 1.0),
        std::invalid_argument
    );
}

TEST(BlackScholesTest, RejectsInvalidVolatility)
{
    EXPECT_THROW(
        quant::black_scholes_call(
            100.0, 100.0, 0.05, 0.0, 1.0),
        std::invalid_argument
    );
}