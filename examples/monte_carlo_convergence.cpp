#include "quant/monte_carlo.hpp"
#include "quant/black_scholes.hpp"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

int main()
{
    constexpr double spot = 100.0;
    constexpr double strike = 100.0;
    constexpr double rate = 0.05;
    constexpr double volatility = 0.20;
    constexpr double maturity = 1.0;

    constexpr std::uint64_t seed = 42;

    const std::vector<std::uint64_t> simulation_counts = {
        1000,
        5000,
        10000,
        50000,
        100000,
        500000,
        1000000,
        5000000
    };

    const double reference_call =
        quant::black_scholes_call(
            spot, strike, rate, volatility, maturity);

    const double reference_put =
        quant::black_scholes_put(
            spot, strike, rate, volatility, maturity);

    std::ofstream csv("monte_carlo_convergence.csv");

    if (!csv)
    {
        std::cerr << "Could not open CSV output file.\n";
        return 1;
    }

    csv << "simulations,"
        << "call_price,call_error,call_se,"
        << "call_ci_lower,call_ci_upper,call_order,"
        << "put_price,put_error,put_se,"
        << "put_ci_lower,put_ci_upper,put_order\n";

    std::cout << std::fixed << std::setprecision(6);

    std::cout << "Black-Scholes reference prices\n";
    std::cout << "Call: " << reference_call << '\n';
    std::cout << "Put:  " << reference_put << "\n\n";

    std::cout
        << std::setw(12) << "Simulations"
        << std::setw(13) << "MC Call"
        << std::setw(13) << "Call Error"
        << std::setw(13) << "Call SE"
        << std::setw(13) << "MC Put"
        << std::setw(13) << "Put Error"
        << std::setw(13) << "Put SE"
        << std::setw(12) << "Call Order"
        << std::setw(12) << "Put Order"
        << '\n';

    std::cout << std::string(114, '-') << '\n';

    double previous_call_se = 0.0;
    double previous_put_se = 0.0;
    std::uint64_t previous_simulations = 0;

    for (const auto simulations : simulation_counts)
    {
        const auto call = quant::monte_carlo_price(
            spot, strike, rate, volatility, maturity,
            simulations, quant::OptionType::Call, seed);

        const auto put = quant::monte_carlo_price(
            spot, strike, rate, volatility, maturity,
            simulations, quant::OptionType::Put, seed);

        const double call_error =
            std::abs(call.price - reference_call);

        const double put_error =
            std::abs(put.price - reference_put);

        double call_order = 0.0;
        double put_order = 0.0;

        if (previous_simulations > 0)
        {
            const double log_ratio =
                std::log(
                    static_cast<double>(simulations) /
                    static_cast<double>(previous_simulations));

            call_order =
                std::log(previous_call_se / call.standard_error)
                / log_ratio;

            put_order =
                std::log(previous_put_se / put.standard_error)
                / log_ratio;
        }

        std::cout
            << std::setw(12) << simulations
            << std::setw(13) << call.price
            << std::setw(13) << call_error
            << std::setw(13) << call.standard_error
            << std::setw(13) << put.price
            << std::setw(13) << put_error
            << std::setw(13) << put.standard_error;

        if (previous_simulations == 0)
        {
            std::cout
                << std::setw(12) << "-"
                << std::setw(12) << "-";
        }
        else
        {
            std::cout
                << std::setw(12) << call_order
                << std::setw(12) << put_order;
        }

        std::cout << '\n';

        // Export numerical results.
        csv << std::setprecision(12)
            << simulations << ','
            << call.price << ','
            << call_error << ','
            << call.standard_error << ','
            << call.confidence_interval_lower << ','
            << call.confidence_interval_upper << ',';

        if (previous_simulations == 0)
            csv << ',';
        else
            csv << call_order << ',';

        csv << put.price << ','
            << put_error << ','
            << put.standard_error << ','
            << put.confidence_interval_lower << ','
            << put.confidence_interval_upper << ',';

        if (previous_simulations == 0)
            csv << '\n';
        else
            csv << put_order << '\n';

        previous_call_se = call.standard_error;
        previous_put_se = put.standard_error;
        previous_simulations = simulations;
    }

    csv.close();

    std::cout << "\nResults saved to "
              << "monte_carlo_convergence.csv\n";

    return 0;
}