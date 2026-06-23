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

'''
start = 30      
step = 6       
length = 8     

arr = [start + i*step for i in range(length)]

z_vals = np.array([4.1, 4.2, 4.3, 4.4, 4.5, 4.6, 4.7, 4.8])
'''

z_vals = np.array([2.056008935464906, 2.624206629061214, 3.190114834094382, 3.799517068220771, 4.47, 5.27])

arr = [64]

rounded_z_vals = np.round(z_vals, 3)

M_vals = [2, 3, 4, 5, 6, 7]

'''
for L in arr:
    for z in rounded_z_vals:
        for M in M_vals:
            sp = {"z": float(z), "M": M, "L": L, "lat": "square"}
            job = project.open_job(sp).init()
            print(job)
'''

for M, z in zip(M_vals, rounded_z_vals):
    sp = {"z": float(z), "M": M, "L": 64, "lat": "square"}
    job = project.open_job(sp)
    job.init()
    print(job)

for str in ["crystal", "density", "demixed"]:
        files = glob.glob(os.path.join("/home/tashfiq/wr_lattice/data/sampling/" + str, "*"))
        for f in files:
            if os.path.isfile(f): 
                os.remove(f)

print(arr)