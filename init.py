# init.py for wr_lattice
import numpy as np
import signac
import os
import shutil
import glob

project = signac.init_project("/home/tashfiq/wr_lattice/data")

directory = "/home/tashfiq/wr_lattice/data/workspace"

for item in os.listdir(directory):
    item_path = os.path.join(directory, item)
    if os.path.isdir(item_path):
        shutil.rmtree(item_path)


arr = [30, 36, 42, 48, 52]


z_vals = np.round(np.linspace(5.42, 5.52, 16), 3)

M_vals = [7]
R = 70 


for L in arr:
    for z in z_vals:
        for M in M_vals:
            for r in range(1, R+1): 
                sp = {"z": float(z), "M": M, "L": L, "lat": "square", "run": r}
                job = project.open_job(sp).init()
                print(job)


for str in ["crystal", "density", "demixed"]:
        files = glob.glob(os.path.join("/home/tashfiq/wr_lattice/data/sampling/" + str, "*"))
        for f in files:
            if os.path.isfile(f): 
                os.remove(f)

print(arr)