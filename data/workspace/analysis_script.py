import matplotlib.pyplot as plt
import numpy as np
import signac
import os

arr = [10, 20, 30, 40]

project = signac.get_project("/home/tashfiq/wr_lattice/data/workspace")

fig, ax = plt.subplots()

M = 0

for L in arr:
    z_values = []
    cum_values = []
    err_values = []
    for job in project:
        if job.sp.L == L:
            z = job.sp.z
            z_values.append(z)
            
            cum = job.fn("cumulant.txt")
            err = job.fn("error_bars.txt")
            
            # read cumulant
            if not os.path.exists(cum):
                cum_values.append(-1)
                err_values.append(-1)
            else:
                with open(cum) as f:
                    cum_values.append(float(f.readline()))
                # read error bar (if available)
                if os.path.exists(err):
                    with open(err) as f:
                        err_values.append(float(f.readline()))
                else:
                    err_values.append(0.0)
    
    # after collecting z_values, cum_values, err_values:
    pairs = sorted([(z, c, e) for z, c, e in zip(z_values, cum_values, err_values) if c != -1])
    if pairs:
        zs, cs, es = zip(*pairs)
        ax.errorbar(zs, cs, yerr=es, marker='o', linestyle='-', label=f'L={L}', capsize=3)

plt.ylim(0, 0.7)
plt.xlim(0.5,1)

plt.legend()
plt.xlabel("z")
plt.ylabel(r'$U_L$')

plt.savefig(f"cumulant_plot_M{M}.png")