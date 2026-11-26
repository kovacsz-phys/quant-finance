#pragma once

namespace quant {

double implied_volatility_call(
    double market_price,
    double spot,
    double strike,
    double rate,
    double maturity,
    double initial_volatility = 0.20
);

double implied_volatility_put(
    double market_price,
    double spot,
    double strike,
    double rate,
    double maturity,
    double initial_volatility = 0.20
);

}