#include "quant/binomial.hpp"
#include "quant/black_scholes.hpp"

#include <gtest/gtest.h>
#include <cmath>
#include <stdexcept>

namespace {

constexpr double S = 100.0;
constexpr double K = 100.0;
constexpr double r = 0.05;
constexpr double sigma = 0.20;
constexpr double T = 1.0;

constexpr int STEPS = 1000;

} // anonymous namespace


TEST(BinomialTest, EuropeanCallPrice)
{
    const double price = quant::crr_binomial_price(
        S, K, r, sigma, T, STEPS,
        quant::OptionType::Call,
        quant::ExerciseStyle::European);

    EXPECT_NEAR(price, 10.450584, 0.02);
}


TEST(BinomialTest, EuropeanPutPrice)
{
    const double price = quant::crr_binomial_price(
        S, K, r, sigma, T, STEPS,
        quant::OptionType::Put,
        quant::ExerciseStyle::European);

    EXPECT_NEAR(price, 5.573526, 0.02);
}


TEST(BinomialTest, ConvergesToBlackScholesCall)
{
    const double binomial_price = quant::crr_binomial_price(
        S, K, r, sigma, T, 2000,
        quant::OptionType::Call,
        quant::ExerciseStyle::European);

    const double analytical_price =
        quant::black_scholes_call(S, K, r, sigma, T);

    EXPECT_NEAR(binomial_price, analytical_price, 0.01);
}


TEST(BinomialTest, ConvergesToBlackScholesPut)
{
    const double binomial_price = quant::crr_binomial_price(
        S, K, r, sigma, T, 2000,
        quant::OptionType::Put,
        quant::ExerciseStyle::European);

    const double analytical_price =
        quant::black_scholes_put(S, K, r, sigma, T);

    EXPECT_NEAR(binomial_price, analytical_price, 0.01);
}


TEST(BinomialTest, EuropeanPutCallParity)
{
    const double call = quant::crr_binomial_price(
        S, K, r, sigma, T, STEPS,
        quant::OptionType::Call,
        quant::ExerciseStyle::European);

    const double put = quant::crr_binomial_price(
        S, K, r, sigma, T, STEPS,
        quant::OptionType::Put,
        quant::ExerciseStyle::European);

    const double parity =
        S - K * std::exp(-r * T);

    EXPECT_NEAR(call - put, parity, 1e-10);
}


TEST(BinomialTest, AmericanCallEqualsEuropeanCall)
{
    const double european = quant::crr_binomial_price(
        S, K, r, sigma, T, STEPS,
        quant::OptionType::Call,
        quant::ExerciseStyle::European);

    const double american = quant::crr_binomial_price(
        S, K, r, sigma, T, STEPS,
        quant::OptionType::Call,
        quant::ExerciseStyle::American);

    EXPECT_NEAR(american, european, 1e-10);
}


TEST(BinomialTest, AmericanPutAtLeastAsValuableAsEuropeanPut)
{
    const double european = quant::crr_binomial_price(
        S, K, r, sigma, T, STEPS,
        quant::OptionType::Put,
        quant::ExerciseStyle::European);

    const double american = quant::crr_binomial_price(
        S, K, r, sigma, T, STEPS,
        quant::OptionType::Put,
        quant::ExerciseStyle::American);

    EXPECT_GE(american, european);
}


TEST(BinomialTest, RejectsZeroSteps)
{
    EXPECT_THROW(
        quant::crr_binomial_price(
            S, K, r, sigma, T, 0,
            quant::OptionType::Call,
            quant::ExerciseStyle::European),
        std::invalid_argument);
}


TEST(BinomialTest, RejectsNegativeVolatility)
{
    EXPECT_THROW(
        quant::crr_binomial_price(
            S, K, r, -0.20, T, STEPS,
            quant::OptionType::Call,
            quant::ExerciseStyle::European),
        std::invalid_argument);
}


TEST(BinomialTest, RejectsInvalidRiskNeutralProbability)
{
    EXPECT_THROW(
        quant::crr_binomial_price(
            S, K, -1.0, 0.01, T, 1,
            quant::OptionType::Call,
            quant::ExerciseStyle::European),
        std::invalid_argument);
}