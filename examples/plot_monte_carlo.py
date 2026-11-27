import csv
import matplotlib.pyplot as plt

filename = "monte_carlo_convergence.csv"

simulations = []
call_se = []
put_se = []

with open(filename, "r", newline="") as file:
    reader = csv.DictReader(file)

    for row in reader:
        simulations.append(int(row["simulations"]))
        call_se.append(float(row["call_se"]))
        put_se.append(float(row["put_se"]))

# Theoretical O(1/sqrt(M)) reference curves.
call_reference = [
    call_se[0] * (simulations[0] / n) ** 0.5
    for n in simulations
]

put_reference = [
    put_se[0] * (simulations[0] / n) ** 0.5
    for n in simulations
]

plt.figure(figsize=(9, 6))

plt.loglog(
    simulations,
    call_se,
    "o-",
    label="Monte Carlo call"
)

plt.loglog(
    simulations,
    put_se,
    "s-",
    label="Monte Carlo put"
)

plt.loglog(
    simulations,
    call_reference,
    "--",
    label=r"Call reference $O(M^{-1/2})$"
)

plt.loglog(
    simulations,
    put_reference,
    "--",
    label=r"Put reference $O(M^{-1/2})$"
)

plt.xlabel("Number of simulations")
plt.ylabel("Standard error")

plt.title("Monte Carlo Pricing Convergence")

plt.grid(True, which="both", linestyle="--", alpha=0.5)

plt.legend()
plt.tight_layout()

plt.savefig(
    "monte_carlo_convergence.png",
    dpi=200
)

plt.show()

print("Plot saved to monte_carlo_convergence.png")