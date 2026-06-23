import signac
import argparse
import numpy as np
import os
import pandas as pd


if __name__ == '__main__':

    project = signac.get_project("/home/tashfiq/wr_lattice/data/workspace")

    for job in project:
        if job.sp.L == 50:
            L = job.sp.L
            M = job.sp.M
            z = job.sp.z
            lat = job.sp.lat

            data_crystal = []
            data_density = []
            data_demixed = []

            for param in ["crystal", "density", "demixed"]:
                filename = param + "_L" + str(L) + "_M" + str(M) + "_z" + f"{z:.3f}".replace('.', '-') + "_" + lat + ".txt"
                pathname = "/home/tashfiq/wr_lattice/data/sampling/" + param + "/" + filename

                with open(pathname, "r") as f:                  # get equilibriated system's nodes from main.cpp file, to be handled by NetworkX to display network graph
                    for x in f:
                        if param == "crystal":
                            data_crystal.append(float(x))
                        if param == "density":
                            data_density.append(float(x))
                        if param == "demixed":
                            data_demixed.append(float(x))
            
            df = pd.DataFrame({
                "crystal": data_crystal,
                "density": data_density,
                "demixed": data_demixed
            })
                
            df = df.iloc[20000:]
            
            dirr = "/home/tashfiq/wr_lattice/data/sampling/var"
            os.makedirs(dirr, exist_ok=True)
            path = os.path.join(dirr, f"var_L{L}_M{M}_z{z:.2f}_3000000.txt")

            with open(path, "w") as f:
                f.write(str(df["demixed"].std()))
       






            