# init.py for wr_lattice
import numpy as np
import signac

project = signac.init_project("/home/tashfiq/wr_lattice/data")

arr = [5, 10, 15, 20, 25, 30, 35, 40, 45, 50]
z_vals = np.linspace(0.1,5,8)
rounded_z_vals = np.round(z_vals, 2)

for L in arr:
    for z in rounded_z_vals:
        sp = {"z": float(z), "M": 2, "L": L, "lat": "hexagonal"}
        job = project.open_job(sp).init()
        print(job)
