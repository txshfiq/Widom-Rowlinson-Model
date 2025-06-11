# init.py for wr_lattice
import numpy
import signac

project = signac.init_project("/home/tashfiq/wr_lattice/data")

for z in numpy.linspace(0.5,1,6):
    sp = {"z": float(z), "M": 9, "L": 30}
    job = project.open_job(sp).init()
    print(job)