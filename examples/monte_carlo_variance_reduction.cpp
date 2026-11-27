#include "quant/monte_carlo.hpp"
#include "quant/black_scholes.hpp"

#include <chrono>
#include <iomanip>
#include <iostream>

int main()
{
    constexpr double spot = 100.0;
    constexpr double strike = 100.0;
    constexpr double rate = 0.05;
    constexpr double volatility = 0.20;
    constexpr double maturity = 1.0;

    constexpr std::uint64_t simulations = 1000000;
    constexpr std::uint64_t seed = 42;

    std::cout << std::fixed << std::setprecision(6);

    const double reference_call = quant::black_scholes_call(
        spot, strike, rate, volatility, maturity);

    const double reference_put = quant::black_scholes_put(
        spot, strike, rate, volatility, maturity);

    for (const auto option_type :
         {quant::OptionType::Call, quant::OptionType::Put})
    {
        const char* name =
            option_type == quant::OptionType::Call ? "CALL" : "PUT";

        const double reference =
            option_type == quant::OptionType::Call
                ? reference_call
                : reference_put;

        const auto start_standard = std::chrono::steady_clock::now();

        const auto standard = quant::monte_carlo_price(
            spot, strike, rate, volatility, maturity,
            simulations, option_type, seed, false);

        const auto end_standard = std::chrono::steady_clock::now();

        const auto start_antithetic = std::chrono::steady_clock::now();

        const auto antithetic = quant::monte_carlo_price(
            spot, strike, rate, volatility, maturity,
            simulations, option_type, seed, true);

        const auto end_antithetic = std::chrono::steady_clock::now();

        const double standard_time =
            std::chrono::duration<double, std::milli>(
                end_standard - start_standard).count();

        const double antithetic_time =
            std::chrono::duration<double, std::milli>(
                end_antithetic - start_antithetic).count();

        const double variance_reduction =
            1.0 -
            (antithetic.standard_error * antithetic.standard_error) /
            (standard.standard_error * standard.standard_error);

        const double efficiency_ratio =
            (standard.standard_error * standard.standard_error) /
            (antithetic.standard_error * antithetic.standard_error);

        std::cout << "\n" << name << " OPTION\n";
        std::cout << "---------------------------------------------\n";

        std::cout << "Black-Scholes reference: "
                  << reference << "\n\n";

        std::cout << std::setw(24) << "Standard MC"
                  << std::setw(24) << "Antithetic MC\n";

        std::cout << std::setw(24) << standard.price
                  << std::setw(24) << antithetic.price << '\n';

        std::cout << std::setw(24) << standard.standard_error
                  << std::setw(24) << antithetic.standard_error << '\n';

        std::cout << "\nAbsolute pricing errors:\n";

        std::cout << "Standard:   "
                  << std::abs(standard.price - reference) << '\n';

        std::cout << "Antithetic: "
                  << std::abs(antithetic.price - reference) << '\n';

        std::cout << "\nVariance reduction: "
                  << variance_reduction * 100.0 << "%\n";

        std::cout << "Variance efficiency ratio: "
                  << efficiency_ratio << "x\n";

        std::cout << "\nExecution time:\n";

        std::cout << "Standard MC:   "
                  << standard_time << " ms\n";

        std::cout << "Antithetic MC: "
                  << antithetic_time << " ms\n";
    }

    return 0;
}