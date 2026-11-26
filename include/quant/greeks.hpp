#pragma once

namespace quant {

struct OptionGreeks {
    double delta;
    double gamma;
    double vega;
    double theta;
    double rho;
};

OptionGreeks black_scholes_call_greeks(
    double spot,
    double strike,
    double rate,
    double volatility,
    double maturity
);

OptionGreeks black_scholes_put_greeks(
    double spot,
    double strike,
    double rate,
    double volatility,
    double maturity
);

}