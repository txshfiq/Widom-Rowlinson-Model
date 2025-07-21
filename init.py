# init.py for wr_lattice
import numpy as np
import signac

project = signac.init_project("/home/tashfiq/wr_lattice/data")

arr = [10, 18, 26, 32, 40]
z_vals = np.linspace(9.1,10,10)
rounded_z_vals = np.round(z_vals, 2)

for L in arr:
    for z in rounded_z_vals:
        sp = {"z": float(z), "M": 8, "L": L, "lat": "hexagonal"}
        job = project.open_job(sp).init()
        print(job)


'''
Either one of these for multiple of 3

arr = [9, 12, 15, 18, 21, 24, 27, 30, 33, 36]
arr = [12, 15, 18, 21, 24, 27, 30, 33, 36, 39]
arr = [15, 18, 21, 24, 27, 30, 33, 36, 39, 42]
'''