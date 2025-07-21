import matplotlib.pyplot as plt
import numpy as np
import signac
import os


arr = [10, 18, 26, 32]

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
            if not os.path.exists(cum):
                cum_values.append(-1)
            else:
                with open(cum) as f:
                    cum_values.append(float(f.readline()))
    
    # after collecting z_values and cum_values:
    pairs = sorted([(z, c) for z, c in zip(z_values, cum_values) if c != -1])
    if pairs:
        zs, cs = zip(*pairs)
        ax.plot(zs, cs, marker='o', linestyle='-', label=f'L={L}')

plt.ylim(0, 0.8)
plt.xlim(9.1,9.3)

plt.legend()

plt.xlabel("z")
plt.ylabel(r'$U_L$')
plt.savefig(f"cumulant_plot_M{M}.png")