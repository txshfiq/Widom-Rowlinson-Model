import signac
import argparse
from pymbar import timeseries
import numpy as np

def output_cumulant(job, cumulant):
    with open(job.fn("cumulant.txt"), "w") as file:
        file.write(str(cumulant) + "\n")

if __name__ == '__main__':

    parser = argparse.ArgumentParser()

    parser.add_argument(
        "--num1", "-z",
        type=float,
        required=True,
        help="Fugacity of system"
    )
    parser.add_argument(
        "--num2", "-M",
        type=int,
        required=True,
        help="Number of species in system"
    )
    parser.add_argument(
        "--num3", "-L",
        type=int,
        required=True,
        help="Size of lattice (L x L)"
    )
    parser.add_argument(
        "--str1", "-lat",
        type=str,
        required=True,
        help="Type of Lattice"
    )

    parser.add_argument('--action', required=True)
    parser.add_argument('directories', nargs='+')

    args = parser.parse_args()

    # Open the signac jobs
    project = signac.get_project("/home/tashfiq/wr_lattice/data/workspace")

    L = args.num3
    M = args.num2
    z = args.num1
    lat = args.str1



    dt = "crystal"

    data = []

    data_file = "data/sampling/" + dt + "/" + dt + "_L" + str(L) + "_M" + str(M) + "_z" + f"{z:.2f}".replace('.', '-') + "_" + lat + ".txt"

    with open(data_file) as f:                  # get equilibriated system's nodes from main.cpp file, to be handled by NetworkX to display network graph
        for x in f:
            data.append(int(x))

    t0, g, Neff_max = timeseries.detectEquilibration(data)
    equilibrated_data = data[t0:]
    uncorr_indices = timeseries.subsample_correlated_data(equilibrated_data)
    uncorr_data = equilibrated_data[uncorr_indices]

    s_2 = [i**2 for i in uncorr_data]
    s_4 = [i**4 for i in uncorr_data]

    U_L = 1 - (1/3)*(np.mean(s_4)/(np.mean(s_2))**2)
    print(U_L)

    for job in project:
        if job.sp.L == L and job.sp.M == M and job.sp.z == z:
            output_cumulant(job, U_L)

    # Call the action
    # globals()[args.action](*jobs)