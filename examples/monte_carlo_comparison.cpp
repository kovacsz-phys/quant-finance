#include "quant/monte_carlo.hpp"
#include "quant/black_scholes.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

using Clock = std::chrono::steady_clock;

struct BenchmarkResult
{
    double median_time_ms;
    double price;
    double standard_error;
};

BenchmarkResult benchmark(
    double spot,
    double strike,
    double rate,
    double volatility,
    double maturity,
    std::uint64_t simulations,
    quant::OptionType option_type,
    std::uint64_t seed,
    bool antithetic,
    int repetitions)
{
    // Statistical estimate using a fixed seed.
    const auto result = quant::monte_carlo_price(
        spot,
        strike,
        rate,
        volatility,
        maturity,
        simulations,
        option_type,
        seed,
        antithetic);

    // Independent timing measurements.
    std::vector<double> timings;
    timings.reserve(repetitions);

    for (int i = 0; i < repetitions; ++i)
    {
        const auto start = Clock::now();

        const auto timing_result = quant::monte_carlo_price(
            spot,
            strike,
            rate,
            volatility,
            maturity,
            simulations,
            option_type,
            seed + static_cast<std::uint64_t>(i),
            antithetic);

        const auto end = Clock::now();

        const double elapsed =
            std::chrono::duration<double, std::milli>(
                end - start).count();

        timings.push_back(elapsed);
    }

    std::sort(timings.begin(), timings.end());

    const std::size_t middle = timings.size() / 2;

    double median = timings[middle];

    if (timings.size() % 2 == 0)
    {
        median =
            0.5 * (timings[middle - 1] + timings[middle]);
    }

    return {
        median,
        result.price,
        result.standard_error
    };
}

int main()
{
    constexpr double spot = 100.0;
    constexpr double strike = 100.0;
    constexpr double rate = 0.05;
    constexpr double volatility = 0.20;
    constexpr double maturity = 1.0;

    constexpr std::uint64_t seed = 42;
    constexpr int repetitions = 20;

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

    std::ofstream csv("monte_carlo_comparison.csv");

    if (!csv)
    {
        std::cerr << "Could not open CSV output file.\n";
        return 1;
    }

    csv << "option,simulations,"
        << "standard_price,standard_error,standard_abs_error,"
        << "standard_order,standard_time_ms,"
        << "antithetic_price,antithetic_error,antithetic_abs_error,"
        << "antithetic_order,antithetic_time_ms,"
        << "variance_reduction,efficiency_ratio,speedup\n";

    std::cout << std::fixed << std::setprecision(6);

    std::cout << "Monte Carlo estimator comparison\n";
    std::cout << "Repetitions per measurement: "
              << repetitions << "\n\n";

    for (const auto option_type :
         {quant::OptionType::Call, quant::OptionType::Put})
    {
        const std::string option_name =
            option_type == quant::OptionType::Call
                ? "CALL"
                : "PUT";

        const double reference =
            option_type == quant::OptionType::Call
                ? quant::black_scholes_call(
                    spot, strike, rate, volatility, maturity)
                : quant::black_scholes_put(
                    spot, strike, rate, volatility, maturity);

        std::cout << "\n" << option_name
                  << " OPTION\n";

        std::cout << "Black-Scholes reference: "
                  << reference << "\n\n";

        std::cout
            << std::setw(12) << "Simulations"
            << std::setw(13) << "Standard SE"
            << std::setw(13) << "Anti SE"
            << std::setw(13) << "Variance %"
            << std::setw(13) << "Efficiency"
            << std::setw(13) << "Speedup"
            << '\n';

        std::cout << std::string(77, '-') << '\n';

        double previous_standard_se = 0.0;
        double previous_antithetic_se = 0.0;
        std::uint64_t previous_simulations = 0;

        for (const auto simulations : simulation_counts)
        {
            const auto standard = benchmark(
                spot, strike, rate, volatility, maturity,
                simulations, option_type, seed,
                false, repetitions);

            const auto antithetic = benchmark(
                spot, strike, rate, volatility, maturity,
                simulations, option_type, seed,
                true, repetitions);

            const double variance_reduction =
                1.0 -
                (antithetic.standard_error *
                 antithetic.standard_error) /
                (standard.standard_error *
                 standard.standard_error);

            const double efficiency_ratio =
                (standard.standard_error *
                 standard.standard_error) /
                (antithetic.standard_error *
                 antithetic.standard_error);

            const double speedup =
                standard.median_time_ms /
                antithetic.median_time_ms;

            double standard_order = 0.0;
            double antithetic_order = 0.0;

            if (previous_simulations > 0)
            {
                const double log_ratio =
                    std::log(
                        static_cast<double>(simulations) /
                        static_cast<double>(previous_simulations));

                standard_order =
                    std::log(
                        previous_standard_se /
                        standard.standard_error) / log_ratio;

                antithetic_order =
                    std::log(
                        previous_antithetic_se /
                        antithetic.standard_error) / log_ratio;
            }

            std::cout
                << std::setw(12) << simulations
                << std::setw(13) << standard.standard_error
                << std::setw(13) << antithetic.standard_error
                << std::setw(13) << variance_reduction * 100.0
                << std::setw(13) << efficiency_ratio
                << std::setw(13) << speedup
                << '\n';

            csv << option_name << ','
                << simulations << ','
                << std::setprecision(12)
                << standard.price << ','
                << standard.standard_error << ','
                << std::abs(standard.price - reference) << ',';

            if (previous_simulations == 0)
                csv << ',';
            else
                csv << standard_order << ',';

            csv << standard.median_time_ms << ','
                << antithetic.price << ','
                << antithetic.standard_error << ','
                << std::abs(antithetic.price - reference) << ',';

            if (previous_simulations == 0)
                csv << ',';
            else
                csv << antithetic_order << ',';

            csv << antithetic.median_time_ms << ','
                << variance_reduction << ','
                << efficiency_ratio << ','
                << speedup << '\n';

            previous_standard_se = standard.standard_error;
            previous_antithetic_se = antithetic.standard_error;
            previous_simulations = simulations;
        }
    }

    csv.close();

    std::cout << "\nResults saved to monte_carlo_comparison.csv\n";

    return 0;
}