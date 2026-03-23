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

start = 30      
step = 4       
length = 16     

arr = [start + i*step for i in range(length)]

z_vals = np.linspace(2.59, 2.64, 6)

rounded_z_vals = np.round(z_vals, 2)

for L in arr:
    for z in rounded_z_vals:
        sp = {"z": float(z), "M": 3, "L": L, "lat": "square"}
        job = project.open_job(sp).init()
        print(job)

for str in ["crystal", "density", "demixed"]:
        files = glob.glob(os.path.join("/home/tashfiq/wr_lattice/data/sampling/" + str, "*"))
        for f in files:
            if os.path.isfile(f): 
                os.remove(f)

print(arr)