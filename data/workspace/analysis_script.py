import matplotlib.pyplot as plt
import numpy as np
import signac


arr = [10, 14, 18, 22, 26, 30, 34, 38, 42, 46]

project = signac.get_project("/home/tashfiq/wr_lattice/data/workspace")

fig, ax = plt.subplots()

M = 0

for L in arr:
    z_values = []
    cum_values = []
    for job in project:
        if job.sp.L == L:
            z = job.sp.z
            z_values.append(z)
            cum = job.fn("cumulant.txt")
            M = job.sp.M
            with open(cum) as f:
                cum_values.append(float(f.readline()))
    
    # after collecting z_values and cum_values:
    pairs = sorted(zip(z_values, cum_values))  # sorts by the first element of each tuple
    if pairs:
        zs, cs = zip(*pairs)
        ax.plot(zs, cs, marker='o', linestyle='-', label=f'L={L}')

plt.ylim(0, 0.7)
plt.xlim(0.5, 1)

plt.legend()

plt.xlabel("z")
plt.ylabel(r'$U_L$')
plt.savefig(f"cumulant_plot_M{M}.png")