#include "quant/binomial.hpp"
#include "quant/black_scholes.hpp"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>
#include <fstream>

int main()
{
    constexpr double spot = 100.0;
    constexpr double strike = 100.0;
    constexpr double rate = 0.05;
    constexpr double volatility = 0.20;
    constexpr double maturity = 1.0;

    const std::vector<int> step_counts = {
        10, 25, 50, 100, 200, 500, 1000, 2000
    };

    const double bs_call = quant::black_scholes_call(
        spot, strike, rate, volatility, maturity);

    const double bs_put = quant::black_scholes_put(
        spot, strike, rate, volatility, maturity);

    std::cout << std::fixed << std::setprecision(8);

    std::cout << "Black-Scholes reference prices\n";
    std::cout << "Call: " << bs_call << '\n';
    std::cout << "Put:  " << bs_put << "\n\n";

    std::cout
        << std::setw(8) << "Steps"
        << std::setw(16) << "CRR Call"
        << std::setw(16) << "Call Error"
        << std::setw(16) << "CRR Put"
        << std::setw(16) << "Put Error"
        << std::setw(12) << "Order"
        << '\n';

    std::cout << std::string(84, '-') << '\n';

    double previous_error = 0.0;
    int previous_steps = 0;

    std::ofstream csv("binomial_convergence.csv");

    if (!csv) {
        std::cerr << "Error: Could not create CSV file.\n";
        return 1;
    }

    csv << "steps,call_error,put_error\n";

    for (int steps : step_counts) {

        const double crr_call = quant::crr_binomial_price(
            spot, strike, rate, volatility, maturity,
            steps, quant::OptionType::Call,
            quant::ExerciseStyle::European);

        const double crr_put = quant::crr_binomial_price(
            spot, strike, rate, volatility, maturity,
            steps, quant::OptionType::Put,
            quant::ExerciseStyle::European);

        const double call_error = std::abs(crr_call - bs_call);
        const double put_error = std::abs(crr_put - bs_put);

        csv << steps << ","
            << call_error << ","
            << put_error << "\n";
            
        std::cout
            << std::setw(8) << steps
            << std::setw(16) << crr_call
            << std::setw(16) << call_error
            << std::setw(16) << crr_put
            << std::setw(16) << put_error;

        if (previous_steps > 0 && call_error > 0.0) {

            const double order =
                std::log(previous_error / call_error) /
                std::log(
                    static_cast<double>(steps) / previous_steps);

            std::cout << std::setw(12) << order;

        } else {
            std::cout << std::setw(12) << "-";
        }

        std::cout << '\n';

        previous_error = call_error;
        previous_steps = steps;
    }

    return 0;
}