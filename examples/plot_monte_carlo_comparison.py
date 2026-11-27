import csv
import matplotlib.pyplot as plt

filename = "monte_carlo_comparison.csv"

data = {
    "CALL": [],
    "PUT": []
}

with open(filename, "r", newline="") as file:
    reader = csv.DictReader(file)

    for row in reader:
        option = row["option"]

        data[option].append({
            "simulations": int(row["simulations"]),
            "standard_se": float(row["standard_error"]),
            "antithetic_se": float(row["antithetic_error"]),
            "variance_reduction": float(row["variance_reduction"]),
            "speedup": float(row["speedup"])
        })

# Plot standard error convergence separately for calls and puts.
for option in ("CALL", "PUT"):
    rows = data[option]

    simulations = [r["simulations"] for r in rows]
    standard_se = [r["standard_se"] for r in rows]
    antithetic_se = [r["antithetic_se"] for r in rows]

    standard_reference = [
        standard_se[0] * (simulations[0] / n) ** 0.5
        for n in simulations
    ]

    antithetic_reference = [
        antithetic_se[0] * (simulations[0] / n) ** 0.5
        for n in simulations
    ]

    plt.figure(figsize=(9, 6))

    plt.loglog(
        simulations,
        standard_se,
        "o-",
        label="Standard MC"
    )

    plt.loglog(
        simulations,
        antithetic_se,
        "s-",
        label="Antithetic MC"
    )

    plt.loglog(
        simulations,
        standard_reference,
        "--",
        label="Standard theoretical O(M^-1/2)"
    )

    plt.loglog(
        simulations,
        antithetic_reference,
        "--",
        label="Antithetic theoretical O(M^-1/2)"
    )

    plt.xlabel("Number of simulated terminal prices")
    plt.ylabel("Standard error")
    plt.title(f"{option} Monte Carlo Convergence")

    plt.grid(True, which="both", linestyle="--", alpha=0.5)
    plt.legend()
    plt.tight_layout()

    output = f"monte_carlo_{option.lower()}_comparison.png"

    plt.savefig(output, dpi=200)
    plt.close()

    print(f"Saved {output}")

# Plot variance reduction.
plt.figure(figsize=(9, 6))

for option in ("CALL", "PUT"):
    rows = data[option]

    plt.plot(
        [r["simulations"] for r in rows],
        [100.0 * r["variance_reduction"] for r in rows],
        "o-",
        label=option
    )

plt.xscale("log")
plt.xlabel("Number of simulated terminal prices")
plt.ylabel("Variance reduction (%)")
plt.title("Antithetic Variance Reduction")

plt.grid(True, which="both", linestyle="--", alpha=0.5)
plt.legend()
plt.tight_layout()

plt.savefig("monte_carlo_variance_reduction.png", dpi=200)
plt.close()

print("Saved monte_carlo_variance_reduction.png")

# Plot execution speedup.
plt.figure(figsize=(9, 6))

for option in ("CALL", "PUT"):
    rows = data[option]

    plt.plot(
        [r["simulations"] for r in rows],
        [r["speedup"] for r in rows],
        "o-",
        label=option
    )

plt.xscale("log")
plt.axhline(1.0, linestyle="--", label="Equal execution time")

plt.xlabel("Number of simulated terminal prices")
plt.ylabel("Standard MC time / Antithetic MC time")
plt.title("Antithetic Execution Speedup")

plt.grid(True, which="both", linestyle="--", alpha=0.5)
plt.legend()
plt.tight_layout()

plt.savefig("monte_carlo_speedup.png", dpi=200)
plt.close()

print("Saved monte_carlo_speedup.png")