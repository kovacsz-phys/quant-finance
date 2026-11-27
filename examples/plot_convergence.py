import csv
from pathlib import Path

import matplotlib.pyplot as plt

# Locate the CSV file in the project root.
project_dir = Path(__file__).resolve().parent.parent
csv_file = project_dir / "binomial_convergence.csv"

steps = []
call_errors = []
put_errors = []

# Read numerical results.
with open(csv_file, newline="") as file:
    reader = csv.DictReader(file)

    for row in reader:
        steps.append(int(row["steps"]))
        call_errors.append(float(row["call_error"]))
        put_errors.append(float(row["put_error"]))

# First-order reference convergence: E(N) proportional to 1/N.
reference_errors = [
    call_errors[0] * steps[0] / n
    for n in steps
]

# Plot convergence.
plt.figure(figsize=(9, 6))

plt.loglog(
    steps,
    call_errors,
    "o-",
    label="CRR Call Error"
)

plt.loglog(
    steps,
    put_errors,
    "s-",
    label="CRR Put Error"
)

plt.loglog(
    steps,
    reference_errors,
    "--",
    label="First-order reference: O(1/N)"
)

plt.xlabel("Number of Binomial Steps")
plt.ylabel("Absolute Pricing Error")

plt.title("CRR Binomial Model Convergence")

plt.grid(True, which="both")
plt.legend()

plt.tight_layout()

output_file = project_dir / "binomial_convergence.png"
plt.savefig(output_file, dpi=300)

print(f"Convergence plot saved to: {output_file}")