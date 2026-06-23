import matplotlib.pyplot as plt
import numpy as np
import signac
import os

z_vals = np.array([2.056008935464906, 2.624206629061214, 3.190114834094382, 3.799517068220771, 4.5, 5.27])
rounded_z_vals = np.round(z_vals, 3)

M_vals = [2, 3, 4, 5, 6, 7]

project = signac.get_project("/home/tashfiq/wr_lattice/data/workspace")

fig, ax = plt.subplots()

taus = []

for M, z in zip(M_vals, rounded_z_vals):
    for job in project:
        if job.sp.M == M and job.sp.z == z:
            autocorr_time = job.fn("autocorr.txt")
            taus.append(float(open(autocorr_time).readline()))

plt.plot(M_vals, taus, marker='o', linestyle='-', label=r'$\tau_{int}$')

plt.title("Integrated Autocorrelation Time vs M")
plt.xlabel(r'$M$')
plt.ylabel(r'$\tau_{int}$')
plt.legend()
plt.savefig("autocorr_time.png", dpi=300, bbox_inches='tight')