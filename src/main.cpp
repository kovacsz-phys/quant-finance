#include "quant/black_scholes.hpp"

#include <iomanip>
#include <iostream>
#include <cmath>
#include <stdexcept>

int main()
{
    try
    {
        // Option parameters
        const double spot = 100.0;
        const double strike = 100.0;
        const double rate = 0.05;
        const double volatility = 0.20;
        const double maturity = 1.0;

        // Calculate option prices
        const double call_price =
            quant::black_scholes_call(
                spot, strike, rate, volatility, maturity);

        const double put_price =
            quant::black_scholes_put(
                spot, strike, rate, volatility, maturity);

        // Put-call parity
        const double parity_difference =
            spot - strike * std::exp(-rate * maturity);

        // Display results
        std::cout << std::fixed << std::setprecision(6);

        std::cout << "Black-Scholes Option Pricing\n";
        std::cout << "-----------------------------\n";

        std::cout << "Spot price:      " << spot << '\n';
        std::cout << "Strike price:    " << strike << '\n';
        std::cout << "Interest rate:   " << rate << '\n';
        std::cout << "Volatility:      " << volatility << '\n';
        std::cout << "Maturity:        " << maturity << '\n';

        std::cout << "\nEuropean call:   "
                  << call_price << '\n';

        std::cout << "European put:    "
                  << put_price << '\n';

        std::cout << "\nPut-call parity:\n";

        std::cout << "Call - Put:      "
                  << call_price - put_price << '\n';

        std::cout << "S - K*exp(-rT):  "
                  << parity_difference << '\n';

        std::cout << "Parity satisfied: "
                  << (std::abs(
                        call_price - put_price - parity_difference
                    ) < 1e-10 ? "YES" : "NO")
                  << '\n';
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}