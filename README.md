# Quantitative Finance Library

A modern C++20 library for quantitative finance, numerical methods, and financial derivatives pricing.

The project provides implementations of fundamental option pricing models, numerical algorithms, and performance-oriented computational methods. It emphasizes mathematical accuracy, efficient algorithms, modern C++ design, and rigorous testing.

It is being developed as a technical portfolio project demonstrating skills relevant to quantitative development, financial engineering, and high-performance C++ software engineering.

## Features

### 1. Black–Scholes Option Pricing

Analytical pricing of European call and put options using the Black–Scholes model.

Supported functionality:

- European call option pricing
- European put option pricing
- Standard normal cumulative distribution function
- Put–call parity
- Input validation and error handling

For a non-dividend-paying underlying asset, the European call price is

\[
C = S N(d_1)-Ke^{-rT}N(d_2),
\]

and the European put price is

\[
P = Ke^{-rT}N(-d_2)-SN(-d_1),
\]

where

\[
d_1 =
\frac{\ln(S/K)+(r+\sigma^2/2)T}
{\sigma\sqrt{T}},
\qquad
d_2=d_1-\sigma\sqrt{T}.
\]

The implementation evaluates the standard normal cumulative distribution function using `std::erfc`.

### 2. Option Greeks

Analytical calculation of the five standard option sensitivities:

- Delta
- Gamma
- Vega
- Theta
- Rho

Both call and put options are supported.

The implementation is validated using finite-difference approximations, providing an independent numerical check of the analytical derivatives.

### 3. Implied Volatility

Numerical estimation of implied volatility from observed European option prices.

The implementation uses a hybrid Newton–Raphson and bisection algorithm.

Key features:

- Separate call and put implied volatility functions
- Newton–Raphson iteration using analytical Vega
- Bisection fallback when a Newton step is unsuitable
- Option price bound validation
- Convergence tolerances and iteration limits
- Input validation and error handling

The hybrid approach combines the fast convergence of Newton–Raphson with the robustness of bracketed root finding.

### 4. CRR Binomial Option Pricing

Implementation of the Cox–Ross–Rubinstein (CRR) binomial model.

Supported functionality:

- European call and put options
- American call and put options
- Risk-neutral probability calculation
- Backward induction
- Early exercise evaluation for American options

The underlying asset follows a recombining binomial tree:

\[
S_{i,j}=S_0u^jd^{i-j},
\]

where

\[
u=e^{\sigma\sqrt{\Delta t}},
\qquad
d=\frac{1}{u},
\qquad
\Delta t=\frac{T}{N}.
\]

The risk-neutral probability is

\[
p=\frac{e^{r\Delta t}-d}{u-d}.
\]

Option values are calculated by backward induction:

\[
V_{i,j}=e^{-r\Delta t}
\left[pV_{i+1,j+1}+(1-p)V_{i+1,j}\right].
\]

For American options, the value at each node is the maximum of the continuation value and the immediate exercise value.

The backward-induction implementation requires:

- Time complexity: \(O(N^2)\)
- Space complexity: \(O(N)\)

A one-dimensional array is used instead of storing the entire binomial tree.

### 5. Direct European Binomial Pricing

An optimized algorithm for European options evaluates the terminal binomial probability distribution directly instead of performing backward induction.

The European option price is calculated as

\[
V_0=e^{-rT}\sum_{j=0}^{N}
\binom Nj p^j(1-p)^{N-j}
\max(S_0u^jd^{N-j}-K,0)
\]

for a call option. The put implementation uses the corresponding put payoff.

The algorithm calculates the probability at the mode of the binomial distribution using logarithms and evaluates the remaining probabilities through recurrence relations.

Additional numerical techniques include:

- Logarithmic probability calculation using `std::lgamma`
- Stable probability recurrence relations
- Compensated summation
- Constant additional memory usage

This reduces the computational complexity from

\[
O(N^2)
\]

to

\[
O(N),
\]

with \(O(1)\) additional space.

The direct algorithm is specifically intended for European options. The original backward-induction implementation is retained for American options, where early exercise must be considered.

### 6. Numerical Convergence Analysis

The project includes a convergence experiment comparing CRR European option prices against analytical Black–Scholes reference values.

The experiment evaluates pricing errors over increasing numbers of binomial steps.

For the tested parameter set, the numerical results exhibit approximately first-order convergence:

\[
E(N)=O\left(\frac{1}{N}\right).
\]

The convergence data can be exported to CSV and plotted using Python and Matplotlib.

The generated convergence plot is included in the repository as `binomial_convergence.png`.

### 7. Performance Benchmarking

The project includes benchmarks comparing the original backward-induction algorithm with the optimized direct European pricing algorithm.

The following results were obtained on an Intel Core i5-8250U processor using a native optimized Release build.

Compiler optimization flags:

```text
-O3 -DNDEBUG -march=native
```

Benchmark results:

| Steps | Backward (ms) | Direct (ms) | Speedup |
|---:|---:|---:|---:|
| 500 | 0.0230 | 0.0026 | 8.93x |
| 1,000 | 0.0840 | 0.0050 | 16.87x |
| 2,000 | 0.3172 | 0.0123 | 25.82x |
| 3,000 | 1.5451 | 0.0179 | 86.18x |
| 4,000 | 5.4094 | 0.0236 | 229.30x |
| 5,000 | 13.1302 | 0.0288 | **456.02x** |

At 5,000 steps, direct European pricing is approximately 456 times faster than backward induction in this benchmark.

The measured performance is consistent with the expected difference in algorithmic complexity:

| Algorithm | Theoretical complexity | Measured scaling exponent |
|---|---:|---:|
| Backward induction | \(O(N^2)\) | Approximately 2.74 |
| Direct European pricing | \(O(N)\) | Approximately 1.04 |

The measured exponents are empirical results, not changes to the theoretical complexity. Hardware effects, including cache and memory behaviour, influence the observed timings.

The benchmark uses repeated measurements and reports median execution times. Results are hardware- and compiler-dependent.

## Technology Stack

| Technology | Purpose |
|---|---|
| C++20 | Core implementation |
| CMake | Build system |
| Ninja | Build backend |
| GCC / G++ | C++ compiler |
| Google Test | Unit testing |
| Eigen | Numerical computing library |
| Python | Numerical analysis and plotting |
| Matplotlib | Visualization |
| Git | Version control |

## Project Structure

```text
quant-finance/
├── CMakeLists.txt
├── README.md
├── include/
│   └── quant/
│       ├── black_scholes.hpp
│       ├── greeks.hpp
│       ├── implied_volatility.hpp
│       └── binomial.hpp
├── src/
│   ├── black_scholes.cpp
│   ├── greeks.cpp
│   ├── implied_volatility.cpp
│   ├── binomial.cpp
│   ├── binomial_direct.cpp
│   └── main.cpp
├── tests/
│   ├── test_black_scholes.cpp
│   ├── test_greeks.cpp
│   ├── test_implied_volatility.cpp
│   └── test_binomial.cpp
├── examples/
│   ├── binomial_convergence.cpp
│   ├── binomial_benchmark.cpp
│   ├── binomial_comparison.cpp
│   └── plot_convergence.py
├── binomial_convergence.png
└── build/
```

The `build/` and `build-native/` directories contain generated build files and are excluded from version control.

## Requirements

- C++20-compatible compiler (GCC or Clang)
- CMake 3.20 or newer
- Ninja
- Google Test
- Python 3
- Matplotlib (for plotting convergence results)

On Ubuntu, the main dependencies can be installed using:

```bash
sudo apt update

sudo apt install -y \
    build-essential \
    cmake \
    ninja-build \
    libgtest-dev \
    python3 \
    python3-venv \
    python3-pip
```

## Building the Project

Clone the repository:

```bash
git clone https://github.com/kovacsz-phys/quant-finance.git

cd quant-finance
```

Configure a Release build:

```bash
cmake -S . -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=Release
```

Compile:

```bash
cmake --build build -j2
```

Run the example application:

```bash
./build/option_pricing
```

## Running the Tests

Run the complete test suite:

```bash
ctest --test-dir build --output-on-failure
```

The current test suite contains **38 tests** covering:

- Black–Scholes call and put pricing
- Option Greeks
- Finite-difference verification of analytical Greeks
- Implied volatility recovery and invalid input handling
- CRR European and American option pricing
- Put–call parity
- Convergence towards Black–Scholes prices
- Direct pricing versus backward induction
- Direct European pricing convergence and parity
- Invalid risk-neutral probabilities and other error conditions

All 38 tests pass successfully.

## Performance Benchmarking

To configure a native optimized build for the current processor:

```bash
cmake -S . -B build-native \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_FLAGS_RELEASE="-O3 -DNDEBUG -march=native"
```

Compile:

```bash
cmake --build build-native -j2
```

Run the direct-versus-backward-induction comparison:

```bash
./build-native/binomial_comparison
```

Run the original backward-induction benchmark for a specific number of steps:

```bash
./build-native/binomial_benchmark 5000
```

The `-march=native` option enables processor-specific optimizations. Such binaries may not be portable to older