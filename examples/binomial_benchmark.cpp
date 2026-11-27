#include "quant/binomial.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <vector>

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: binomial_benchmark <steps>\n";
        return 1;
    }

    const int steps = std::atoi(argv[1]);

    if (steps <= 0) {
        std::cerr << "Steps must be positive.\n";
        return 1;
    }

    constexpr double spot = 100.0;
    constexpr double strike = 100.0;
    constexpr double rate = 0.05;
    constexpr double volatility = 0.20;
    constexpr double maturity = 1.0;

    constexpr double target_time_ms = 100.0;
    constexpr int repetitions = 7;

    double checksum = 0.0;

    auto calculate_price = [&]() {
        return quant::crr_binomial_price(
            spot,
            strike,
            rate,
            volatility,
            maturity,
            steps,
            quant::OptionType::Call,
            quant::ExerciseStyle::European
        );
    };

    // Warm-up.
    checksum += calculate_price();

    // Estimate execution time.
    const auto start = std::chrono::steady_clock::now();

    checksum += calculate_price();

    const auto end = std::chrono::steady_clock::now();

    const double estimate_ms =
        std::chrono::duration<double, std::milli>(
            end - start
        ).count();

    // Determine batch size.
    const int batch_size = std::max(
        1,
        static_cast<int>(std::ceil(
            target_time_ms /
            std::max(estimate_ms, 0.001)
        ))
    );

    std::vector<double> measurements;

    // Repeated measurements.
    for (int repetition = 0;
         repetition < repetitions;
         ++repetition)
    {
        const auto batch_start =
            std::chrono::steady_clock::now();

        for (int i = 0; i < batch_size; ++i) {
            checksum += calculate_price();
        }

        const auto batch_end =
            std::chrono::steady_clock::now();

        const double elapsed_ms =
            std::chrono::duration<double, std::milli>(
                batch_end - batch_start
            ).count();

        measurements.push_back(elapsed_ms / batch_size);
    }

    // Calculate median.
    std::sort(measurements.begin(), measurements.end());

    const double median_time =
        measurements[measurements.size() / 2];

    const int total_calls = 2 + repetitions * batch_size;

    // Display results.
    std::cout << std::fixed << std::setprecision(3);

    std::cout
        << std::setw(10) << "Steps"
        << std::setw(18) << "Median time (ms)"
        << '\n';

    std::cout << "Batch size: " << batch_size << '\n';
    std::cout << "Total pricing calls: " << total_calls << '\n';

    std::cout << std::string(28, '-') << '\n';

    std::cout
        << std::setw(10) << steps
        << std::setw(18) << median_time
        << '\n';

    std::cout << "\nBenchmark checksum: "
              << checksum << '\n';

    return 0;
}