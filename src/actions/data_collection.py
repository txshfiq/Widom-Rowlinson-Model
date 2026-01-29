import signac
import argparse
import numpy as np
import matplotlib.pyplot as plt
import math
import os

if __name__ == '__main__':

    project = signac.get_project("/home/tashfiq/wr_lattice/data/workspace")

    for job in project:
        if job.sp.L == 30:
            L = job.sp.L
            M = job.sp.M
            z = job.sp.z
            lat = job.sp.lat

            data_crystal = []
            data_density = []
            data_demixed = []

            for param in ["crystal", "density", "demixed"]:
                filename = param + "_L" + str(L) + "_M" + str(M) + "_z" + f"{z:.2f}".replace('.', '-') + "_" + lat + ".txt"
                pathname = "/home/tashfiq/wr_lattice/data/sampling/" + param + "/" + filename

                with open(pathname, "r") as f:                  # get equilibriated system's nodes from main.cpp file, to be handled by NetworkX to display network graph
                    for x in f:
                        if param == "crystal":
                            data_crystal.append(float(x))
                        if param == "density":
                            data_density.append(float(x))
                        if param == "demixed":
                            data_demixed.append(float(x))
            
            
                plt.figure(figsize=(8, 5))
                plt.plot(data_crystal, label='Crystal Order Parameter')
                plt.plot(data_density, label='Density')
                plt.plot(data_demixed, label='Demixed Order Parameter')

                plt.xlabel("Monte Carlo Steps")
                plt.ylabel("Value")
                plt.title(f"Simulation Results (L={L}, M={M}, z={z:.2f}, lat={lat})")
                plt.legend()
                plt.grid(True)
                plt.tight_layout()

                # Save figure to the plots directory
                plot_dir = "/home/tashfiq/wr_lattice/data/sampling/plots"
                os.makedirs(plot_dir, exist_ok=True)
                plot_path = os.path.join(plot_dir, f"plot_L{L}_M{M}_z{z:.2f}_{lat}.png")
                plt.savefig(plot_path)
                plt.close()


        


        




