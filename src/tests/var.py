import os
import re
import pandas as pd
import matplotlib.pyplot as plt

# Folder containing the txt files
FOLDER = "../../data/sampling/var"

# Dictionary to store results
# key   -> (z_value, sweep_amount)
# value -> numerical value extracted from file
data = {}

# Regex to extract z and sweep from filename
# Example filename:
# var_L50_M4_z3.21_1000000.txt
filename_pattern = re.compile(
    r"z(?P<z>[-+]?\d*\.?\d+)_+(?P<sweep>\d+)\.txt$"
)

# Regex to extract numerical value from inside file
# Adjust this depending on your file format
value_pattern = re.compile(r"([-+]?\d*\.?\d+(?:[eE][-+]?\d+)?)")

for filename in os.listdir(FOLDER):

    if not filename.endswith(".txt"):
        continue

    match = filename_pattern.search(filename)

    if not match:
        print(f"Skipping unmatched filename: {filename}")
        continue

    z_value = float(match.group("z"))
    sweep_amount = int(match.group("sweep"))

    filepath = os.path.join(FOLDER, filename)

    with open(filepath, "r") as f:
        contents = f.read()

    # Extract first numerical value from file
    value_match = value_pattern.search(contents)

    if not value_match:
        print(f"No numerical value found in: {filename}")
        continue

    numerical_value = float(value_match.group(1))

    # Store in dictionary using tuple key
    data[(z_value, sweep_amount)] = numerical_value


# Convert to DataFrame for easier analysis
df = pd.DataFrame(
    [(z, sweep, value) for (z, sweep), value in data.items()],
    columns=["z", "sweep", "value"]
)

df = df.sort_values(["z", "sweep"])

# Plot one line per z value
for z_value, group in df.groupby("z"):
    plt.plot(
        group["sweep"],
        group["value"],
        marker="o",
        label=f"z={z_value}"
    )

plt.xlabel("Sweep Amount")
plt.ylabel("Standard deviation of OP (σ)")
plt.title("σ vs Sweep for Different z")
plt.legend()
plt.grid(True)

plt.savefig("var_plot.png", dpi=300, bbox_inches="tight")