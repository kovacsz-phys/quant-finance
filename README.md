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

### 7. Monte Carlo Option Pricing

Implementation of Monte Carlo pricing for European call and put options under the geometric Brownian motion model.

The terminal underlying asset price is simulated as

\[
S_T=S_0\exp\left[
\left(r-\frac{\sigma^2}{2}\right)T
+\sigma\sqrt{T}Z
\right],
\]

where \(Z\) is a standard normally distributed random variable.

The option price is estimated from discounted simulated payoffs:

\[
\hat V_0=
\frac{e^{-rT}}{M}
\sum_{i=1}^{M}
\text{Payoff}(S_T^{(i)}),
\]

where \(M\) is the number of simulated terminal asset prices.

Supported functionality:

- European call and put pricing
- Geometric Brownian motion simulation
- Pseudorandom normal sampling using `std::mt19937_64`
- Reproducible simulations through configurable random seeds
- Online calculation of sample mean and variance
- Standard error estimation
- Approximate 95% confidence intervals
- Input validation and numerical error handling

The implementation uses Welford's online algorithm to calculate the sample variance without storing individual simulation results.

The standard error is calculated as

\[
SE=\frac{s}{\sqrt{M}},
\]

where \(s\) is the sample standard deviation of the discounted payoffs.

The approximate 95% confidence interval is

\[
\hat V_0 \pm 1.96SE.
\]

This provides a measure of the statistical uncertainty associated with the Monte Carlo price estimate.

### 8. Variance Reduction: Antithetic Variates

The Monte Carlo implementation supports antithetic variates as a variance-reduction technique.

Instead of generating independent random numbers for every simulation, the algorithm generates pairs:

\[
Z,\quad -Z.
\]

The corresponding discounted payoffs are averaged:

\[
X_{\text{pair}}=
\frac{1}{2}
\left[
X(Z)+X(-Z)
\right].
\]

The estimator is then calculated using the pair averages as independent observations.

This exploits the negative correlation between paired payoffs to reduce the variance of the price estimator.

The standard error is calculated from the pair-average observations, correctly accounting for the dependence between the two payoffs within each pair.

Antithetic variates require an even number of simulated terminal asset prices.

### 9. Monte Carlo Convergence Analysis

The project includes experiments comparing Monte Carlo estimates against analytical Black–Scholes reference prices.

Simulation counts range from 1,000 to 5,000,000.

The experiments examine:

- Call and put pricing errors
- Standard error as a function of simulation count
- Empirical convergence order
- Comparison with the theoretical Monte Carlo convergence rate

For standard Monte Carlo simulation, the expected convergence relationship is

\[
SE(M)=O(M^{-1/2}),
\]

where \(M\) is the number of simulations.

The numerical experiments demonstrate convergence towards this theoretical rate.

The results are exported to CSV and visualized using Python and Matplotlib.

The generated convergence plot is included as `monte_carlo_convergence.png`.

### 10. Monte Carlo Variance-Reduction Benchmark

A comparative benchmark evaluates standard Monte Carlo against antithetic variates for European call and put options.

The experiment uses the following parameters:

| Parameter | Value |
|---|---:|
| Initial asset price | 100 |
| Strike price | 100 |
| Risk-free interest rate | 5% |
| Volatility | 20% |
| Time to maturity | 1 year |
| Simulation counts | 1,000–5,000,000 |

The benchmark measures median execution time over 20 repetitions and compares the standard errors of the two estimators.

At 5,000,000 simulated terminal asset prices, the following results were obtained:

| Metric | European Call | European Put |
|---|---:|---:|
| Standard MC standard error | 0.006584 | 0.003875 |
| Antithetic standard error | 0.004654 | 0.002964 |
| Variance reduction | 50.04% | 41.47% |
| Variance efficiency ratio | 2.00x | 1.71x |
| Execution speedup | 1.61x | 1.62x |

Variance reduction is calculated as

\[
1-\frac{SE_{\text{antithetic}}^2}
{SE_{\text{standard}}^2}.
\]

The variance efficiency ratio is

\[
\frac{SE_{\text{standard}}^2}
{SE_{\text{antithetic}}^2}.
\]

For the tested parameters, antithetic variates reduce the estimated variance by approximately 50% for the call option and 41% for the put option.

The results demonstrate that variance reduction can improve both statistical and computational efficiency.

These measurements are empirical results for the specified model parameters and implementation, rather than universal performance guarantees.

The benchmark generates the following visualizations:

- `monte_carlo_call_comparison.png` — standard error convergence for call options
- `monte_carlo_put_comparison.png` — standard error convergence for put options
- `monte_carlo_variance_reduction.png` — variance reduction across simulation counts
- `monte_carlo_speedup.png` — execution speedup across simulation counts

## 11. Binomial Performance Benchmarking

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
│       ├── binomial.hpp
│       └── monte_carlo.hpp
├── src/
│   ├── black_scholes.cpp
│   ├── greeks.cpp
│   ├── implied_volatility.cpp
│   ├── binomial.cpp
│   ├── binomial_direct.cpp
│   ├── monte_carlo.cpp
│   └── main.cpp
├── tests/
│   ├── test_black_scholes.cpp
│   ├── test_greeks.cpp
│   ├── test_implied_volatility.cpp
│   ├── test_binomial.cpp
│   └── test_monte_carlo.cpp
├── examples/
│   ├── binomial_convergence.cpp
│   ├── binomial_benchmark.cpp
│   ├── binomial_comparison.cpp
│   ├── monte_carlo_convergence.cpp
│   ├── monte_carlo_variance_reduction.cpp
│   ├── monte_carlo_comparison.cpp
│   ├── plot_convergence.py
│   ├── plot_monte_carlo.py
│   └── plot_monte_carlo_comparison.py
├── binomial_convergence.png
├── monte_carlo_convergence.png
├── monte_carlo_call_comparison.png
├── monte_carlo_put_comparison.png
├── monte_carlo_variance_reduction.png
└── monte_carlo_speedup.png
```

The `build/` and `build-native/` directories contain generated build files and are excluded from version control.

## Requirements

- C++20-compatible compiler (GCC or Clang)
- CMake 3.20 or newer
- Ninja
- Google Test
- Python 3
- Matplotlib (for plotting convergence and benchmark results)

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

The current test suite contains **55 tests** covering:

- Black–Scholes call and put pricing
- Option Greeks
- Finite-difference verification of analytical Greeks
- Implied volatility recovery and invalid input handling
- CRR European and American option pricing
- Put–call parity
- Convergence towards Black–Scholes prices
- Direct pricing versus backward induction
- Direct European pricing convergence and parity
- Monte Carlo call and put pricing
- Reproducibility with fixed random seeds
- Standard error and confidence interval calculations
- Antithetic variates pricing and reproducibility
- Variance reduction for call and put options
- Input validation and error handling

**All 55 tests pass successfully.**

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

### Binomial benchmarks

Run the direct-versus-backward-induction comparison:

```bash
./build-native/binomial_comparison
```

Run the original backward-induction benchmark for a specific number of steps:

```bash
./build-native/binomial_benchmark 5000
```

### Monte Carlo experiments

Run the convergence experiment:

```bash
./build-native/monte_carlo_convergence
```

Run the variance-reduction benchmark:

```bash
./build-native/monte_carlo_variance_reduction
```

Run the standard-versus-antithetic comparison:

```bash
./build-native/monte_carlo_comparison
```

The Monte Carlo comparison generates `monte_carlo_comparison.csv`.

### Plotting results

Create and activate a Python virtual environment:

```bash
python3 -m venv .venv

source .venv/bin/activate

pip install matplotlib
```

Generate the binomial convergence plot:

```bash
python examples/plot_convergence.py
```

Generate the Monte Carlo convergence plot:

```bash
python examples/plot_monte_carlo.py
```

Generate the Monte Carlo comparison plots:

```bash
python examples/plot_monte_carlo_comparison.py
```

The `-march=native` option enables processor-specific optimizations. Such binaries may not be portable to older or different processor architectures.

Benchmark results depend on the processor, compiler, optimization flags, and system load. They should be interpreted as measurements of the tested implementation and environment.

## Testing and Validation

The project uses Google Test for automated unit testing.

Numerical implementations are validated through:

- Analytical reference values
- Put–call parity
- Finite-difference approximations
- Convergence towards analytical solutions
- Cross-validation between independent pricing algorithms
- Reproducibility checks
- Statistical error estimation
- Input validation and error handling

The goal is to combine mathematical correctness with maintainable, efficient C++ implementations.