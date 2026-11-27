cmake_minimum_required(VERSION 3.20)

project(QuantFinance
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Quantitative finance library
add_library(quant_finance
    src/black_scholes.cpp
    src/greeks.cpp
    src/implied_volatility.cpp
    src/binomial.cpp
    src/binomial_direct.cpp
    src/monte_carlo.cpp
)

target_include_directories(quant_finance
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

# Example application
add_executable(option_pricing
    src/main.cpp
)

target_link_libraries(option_pricing
    PRIVATE quant_finance
)

# CRR binomial convergence analysis
add_executable(binomial_convergence
    examples/binomial_convergence.cpp
)

target_link_libraries(binomial_convergence
    PRIVATE quant_finance
)

# CRR binomial performance benchmark
add_executable(binomial_benchmark
    examples/binomial_benchmark.cpp
)

target_link_libraries(binomial_benchmark
    PRIVATE quant_finance
)

# Direct vs. backward-induction performance comparison
add_executable(binomial_comparison
    examples/binomial_comparison.cpp
)

target_link_libraries(binomial_comparison
    PRIVATE quant_finance
)

# Monte Carlo convergence analysis
add_executable(monte_carlo_convergence
    examples/monte_carlo_convergence.cpp
)

target_link_libraries(monte_carlo_convergence
    PRIVATE quant_finance
)

# Monte Carlo variance reduction experiment
add_executable(monte_carlo_variance_reduction
    examples/monte_carlo_variance_reduction.cpp
)

target_link_libraries(monte_carlo_variance_reduction
    PRIVATE quant_finance
)

# Monte Carlo estimator comparison
add_executable(monte_carlo_comparison
    examples/monte_carlo_comparison.cpp
)

target_link_libraries(monte_carlo_comparison
    PRIVATE quant_finance
)

# Unit testing
include(CTest)

if(BUILD_TESTING)

    find_package(GTest REQUIRED)

    add_executable(quant_finance_tests
    tests/test_black_scholes.cpp
    tests/test_greeks.cpp
    tests/test_implied_volatility.cpp
    tests/test_binomial.cpp
    tests/test_monte_carlo.cpp
    )

    target_link_libraries(quant_finance_tests
        PRIVATE
        quant_finance
        GTest::gtest_main
    )

    include(GoogleTest)

    gtest_discover_tests(quant_finance_tests)

endif()