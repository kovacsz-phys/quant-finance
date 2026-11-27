#include "quant/binomial.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

using Clock = std::chrono::steady_clock;

constexpr double SPOT = 100.0;
constexpr double STRIKE = 100.0;
constexpr double RATE = 0.05;
constexpr double VOLATILITY = 0.20;
constexpr double MATURITY = 1.0;

constexpr int REPETITIONS = 7;
constexpr double TARGET_TIME_MS = 100.0;

template <typename Function>
double benchmark(Function&& function)
{
    double checksum = 0.0;

    // Warm-up.
    checksum += function();

    // Estimate execution time.
    auto start = Clock::now();
    checksum += function();
    auto end = Clock::now();

    const double estimate_ms =
        std::chrono::duration<double, std::milli>(
            end - start).count();

    const int batch_size = std::max(
        1,
        static_cast<int>(
            std::ceil(TARGET_TIME_MS /
                      std::max(estimate_ms, 0.001))));

    std::vector<double> measurements;

    for (int repetition = 0;
         repetition < REPETITIONS;
         ++repetition)
    {
        start = Clock::now();

        for (int i = 0; i < batch_size; ++i)
        {
            checksum += function();
        }

        end = Clock::now();

        const double elapsed_ms =
            std::chrono::duration<double, std::milli>(
                end - start).count();

        measurements.push_back(elapsed_ms / batch_size);
    }

    // Median execution time.
    std::sort(measurements.begin(), measurements.end());

    // Keep checksum observable.
    if (!std::isfinite(checksum))
    {
        std::cerr << "Invalid benchmark checksum.\n";
        return 0.0;
    }

    return measurements[measurements.size() / 2];
}

int main()
{
    const std::vector<int> step_counts = {
        500, 1000, 2000, 3000, 4000, 5000
    };

    std::cout << std::fixed << std::setprecision(4);

    std::cout
        << std::setw(10) << "Steps"
        << std::setw(20) << "Backward (ms)"
        << std::setw(20) << "Direct (ms)"
        << std::setw(16) << "Speedup"
        << '\n';

    std::cout << std::string(66, '-') << '\n';

    for (const int steps : step_counts)
    {
        const double backward_time = benchmark([&]() {
            return quant::crr_binomial_price(
                SPOT, STRIKE, RATE, VOLATILITY,
                MATURITY, steps,
                quant::OptionType::Call,
                quant::ExerciseStyle::European);
        });

        const double direct_time = benchmark([&]() {
            return quant::crr_european_price_direct(
                SPOT, STRIKE, RATE, VOLATILITY,
                MATURITY, steps,
                quant::OptionType::Call);
        });

        const double speedup = backward_time / direct_time;

        std::cout
            << std::setw(10) << steps
            << std::setw(20) << backward_time
            << std::setw(20) << direct_time
            << std::setw(16) << speedup << "x"
            << '\n';
    }

    return 0;
}