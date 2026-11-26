#pragma once

namespace quant {

double black_scholes_call(
    double spot,
    double strike,
    double rate,
    double volatility,
    double maturity
);

double black_scholes_put(
    double spot,
    double strike,
    double rate,
    double volatility,
    double maturity
);

}