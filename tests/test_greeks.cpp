#include "quant/black_scholes.hpp"
#include "quant/greeks.hpp"

#include <gtest/gtest.h>
#include <cmath>

namespace {

constexpr double S = 100.0;
constexpr double K = 100.0;
constexpr double r = 0.05;
constexpr double sigma = 0.20;
constexpr double T = 1.0;

constexpr double EPSILON = 1e-5;

} // anonymous namespace


TEST(GreeksTest, CallDelta)
{
    const auto g = quant::black_scholes_call_greeks(
        S, K, r, sigma, T);

    EXPECT_NEAR(g.delta, 0.636831, 1e-5);
}


TEST(GreeksTest, PutDelta)
{
    const auto g = quant::black_scholes_put_greeks(
        S, K, r, sigma, T);

    EXPECT_NEAR(g.delta, -0.363169, 1e-5);
}


TEST(GreeksTest, Gamma)
{
    const auto call = quant::black_scholes_call_greeks(
        S, K, r, sigma, T);

    const auto put = quant::black_scholes_put_greeks(
        S, K, r, sigma, T);

    EXPECT_NEAR(call.gamma, 0.018762, 1e-5);
    EXPECT_NEAR(put.gamma, call.gamma, 1e-12);
}


TEST(GreeksTest, Vega)
{
    const auto g = quant::black_scholes_call_greeks(
        S, K, r, sigma, T);

    EXPECT_NEAR(g.vega, 37.524035, 1e-5);
}


TEST(GreeksTest, Theta)
{
    const auto call = quant::black_scholes_call_greeks(
        S, K, r, sigma, T);

    const auto put = quant::black_scholes_put_greeks(
        S, K, r, sigma, T);

    EXPECT_NEAR(call.theta, -6.414028, 1e-5);
    EXPECT_NEAR(put.theta, -1.657880, 1e-5);
}


TEST(GreeksTest, Rho)
{
    const auto call = quant::black_scholes_call_greeks(
        S, K, r, sigma, T);

    const auto put = quant::black_scholes_put_greeks(
        S, K, r, sigma, T);

    EXPECT_NEAR(call.rho, 53.232482, 1e-5);
    EXPECT_NEAR(put.rho, -41.890461, 1e-5);
}


TEST(GreeksTest, DeltaFiniteDifference)
{
    const double h = 1e-3;

    const double numerical =
        (quant::black_scholes_call(S + h, K, r, sigma, T)
        - quant::black_scholes_call(S - h, K, r, sigma, T))
        / (2.0 * h);

    const auto analytical =
        quant::black_scholes_call_greeks(S, K, r, sigma, T);

    EXPECT_NEAR(analytical.delta, numerical, 1e-7);
}


TEST(GreeksTest, GammaFiniteDifference)
{
    const double h = 1e-2;

    const double numerical =
        (quant::black_scholes_call(S + h, K, r, sigma, T)
        - 2.0 * quant::black_scholes_call(S, K, r, sigma, T)
        + quant::black_scholes_call(S - h, K, r, sigma, T))
        / (h * h);

    const auto analytical =
        quant::black_scholes_call_greeks(S, K, r, sigma, T);

    EXPECT_NEAR(analytical.gamma, numerical, 1e-7);
}


TEST(GreeksTest, VegaFiniteDifference)
{
    const double h = 1e-5;

    const double numerical =
        (quant::black_scholes_call(S, K, r, sigma + h, T)
        - quant::black_scholes_call(S, K, r, sigma - h, T))
        / (2.0 * h);

    const auto analytical =
        quant::black_scholes_call_greeks(S, K, r, sigma, T);

    EXPECT_NEAR(analytical.vega, numerical, 1e-7);
}


TEST(GreeksTest, ThetaFiniteDifference)
{
    const double h = 1e-5;

    // Theta is the negative derivative with respect to maturity.
    const double numerical =
        -(quant::black_scholes_call(S, K, r, sigma, T + h)
        - quant::black_scholes_call(S, K, r, sigma, T - h))
        / (2.0 * h);

    const auto analytical =
        quant::black_scholes_call_greeks(S, K, r, sigma, T);

    EXPECT_NEAR(analytical.theta, numerical, 1e-7);
}


TEST(GreeksTest, RhoFiniteDifference)
{
    const double h = 1e-5;

    const double numerical =
        (quant::black_scholes_call(S, K, r + h, sigma, T)
        - quant::black_scholes_call(S, K, r - h, sigma, T))
        / (2.0 * h);

    const auto analytical =
        quant::black_scholes_call_greeks(S, K, r, sigma, T);

    EXPECT_NEAR(analytical.rho, numerical, 1e-7);
}