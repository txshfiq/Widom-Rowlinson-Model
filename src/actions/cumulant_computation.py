import signac
import argparse
from pymbar import timeseries
import numpy as np
import os
import matplotlib.pyplot as plt
import math

def output_cumulant(job, cumulant):
    with open(job.fn("cumulant.txt"), "w") as file:
        file.write(str(cumulant) + "\n")

def add_error_bars(job, error):
    with open(job.fn("error_bars.txt"), "w") as file:
        file.write(str(error) + "\n")


def plot_block_curve(ax, L, avg, err, ref_value=None):

    ax.errorbar(L, avg, yerr=err, fmt='|', lw=1, capsize=0)
    ax.plot(L, avg, lw=1)

    ax.set_xlabel("Block length L (samples)")
    ax.set_ylabel("Average value")
    ax.margins(x=0.03)

    if ref_value is not None:
        ax.axhline(ref_value, color='gray', linestyle='--', linewidth=1)

    plt.tight_layout()
    plt.savefig("block_curve.png", dpi=300, bbox_inches="tight")  # PNG (raster)

def binder_cumulant(data):
    s_2 = [i**2 for i in data]
    s_4 = [i**4 for i in data]

    U_L = 1 - (1/3)*(np.mean(s_4)/(np.mean(s_2))**2)

    return U_L

def autocorr(j, data):
    var = (np.std(data))**2
    x_bar = np.mean(data)
    covar = 0
    for k in range(len(data) - j):
        covar+=(data[k] - x_bar)*(data[k+j] - x_bar)
    covar/=len(data)

    return covar/var

def block_curve_analysis(data):
    errors = []
    mean_block_avgs = []
    block_sizes = np.arange(100, 4000 + 1, 100)

    for L in block_sizes:
        N_b = math.floor(len(data)/L)
        blocked_data = data[:N_b*L].reshape(N_b, L)
        
        block_vals = [binder_cumulant(blocked_data[i]) for i in range(N_b)]

        ensemble_avg = np.mean(block_vals)
        mean_block_avgs.append(ensemble_avg)
        var = 0
        for i in range(0, N_b-1):
            var += (block_vals[i] - ensemble_avg)**2
        var /= (N_b - 1)
        err = np.sqrt(var / N_b)
        errors.append(err)
    
    fig, ax = plt.subplots(figsize=(5,4))   
    plot_block_curve(ax, block_sizes, mean_block_avgs, errors, binder_cumulant(data))

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

    filename = dt + "_L" + str(L) + "_M" + str(M) + "_z" + f"{z:.2f}".replace('.', '-') + "_" + lat + ".txt"
    pathname = "/home/tashfiq/wr_lattice/data/sampling/crystal/" + filename

    with open(pathname, "r") as f:                  # get equilibriated system's nodes from main.cpp file, to be handled by NetworkX to display network graph
        for x in f:
            data.append(float(x))

    data = np.array(data)

    # t0, g, Neff_max = timeseries.detect_equilibration(data_limit)
    # print(len(data))

    
    data = data[100000:]
    
    '''
    g = timeseries.statistical_inefficiency(data)
    tau_int = (g-1.0) / 2.0

    data = data[::math.floor(tau_int)]
    '''
    
    L_opt = 3000

    N_b = math.floor(len(data)/L_opt)
    blocked_data = data[:N_b*L_opt].reshape(N_b, L_opt)
    block_vals = [binder_cumulant(blocked_data[i]) for i in range(N_b)]
    ensemble_avg = np.mean(block_vals)
    var = 0
    for i in range(0, N_b-1):
        var += (block_vals[i] - ensemble_avg)**2
    var /= (N_b - 1)
    err = np.sqrt(var / N_b)
    
    U_L = binder_cumulant(data)
    
    for job in project:
        if job.sp.L == L and job.sp.M == M and job.sp.z == z:
            output_cumulant(job, U_L)
            add_error_bars(job, err)

    # Call the action
    # globals()[args.action](*jobs)