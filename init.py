# init.py for wr_lattice
import numpy
import signac

project = signac.init_project("/home/tashfiq/wr_lattice/data")

arr = [10, 14, 18, 22, 26, 30, 34, 38, 42, 46]

for L in arr:
    for z in numpy.linspace(0.8,1.3,6):
        sp = {"z": float(z), "M": 8, "L": L}
        job = project.open_job(sp).init()
        print(job)


