import numpy as np # pyright: ignore[reportMissingImports]
import pandas as pd # pyright: ignore[reportMissingModuleSource]
import signac 
import seaborn as sns
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit

def crossing_fit(L_inv, z_c, A, exp):
    return z_c + A*L_inv**(exp)


project = signac.get_project("/home/tashfiq/wr_lattice/data/workspace")
system_sizes = [10, 20, 30, 40, 50, 60]
L_param = np.array([10, 20, 30, 40, 50])
M = 10 ##############
n_bootstraps = 1500
z_min = 0.6 ##############
z_max = 0.7 ##############

bootstrapped_curves = {}
actual_cumulants = {}

for L in system_sizes:
    jobs = project.find_jobs({'M': M, 'L': L})

    jobs_sorted = sorted(jobs, key=lambda job: job.sp.z)
    matrix_L = np.zeros((n_bootstraps, len(jobs_sorted)))
    for i, job in enumerate(jobs_sorted): 
        bs_path = job.fn("bootstrap.txt")

        col_data = np.loadtxt(bs_path)
        matrix_L[:, i] = col_data
    
    actual_cumulants[L] = np.mean(matrix_L, axis=0)
    bootstrapped_curves[L] = matrix_L

errors = np.empty((n_bootstraps, len(system_sizes) - 1))
int_pts = np.array([])


for i in range(len(system_sizes)-1):
    y_data_1 = actual_cumulants[system_sizes[i]]
    y_data_2 = actual_cumulants[system_sizes[i+1]]
    x_data = [job.sp.z for job in sorted(jobs, key=lambda job: job.sp.z)]
    
    z_min_index = x_data.index(z_min)
    z_max_index = x_data.index(z_max)

    y_A_1 = y_data_1[z_min_index]
    y_A_2 = y_data_1[z_max_index]
    y_B_1 = y_data_2[z_min_index]
    y_B_2 = y_data_2[z_max_index]

    mA = (y_A_2 - y_A_1)/(z_max-z_min)
    mB = (y_B_2 - y_B_1)/(z_max-z_min)

    root = (y_B_1 - y_A_1 + z_min*(mA - mB))/(mA-mB)
    
    int_pts = np.append(int_pts, root)

for k in range(n_bootstraps):
    int_pts = np.array([])
    
    for i in range(len(system_sizes)-1):
        y_data_1 = bootstrapped_curves[system_sizes[i]][k, :]
        y_data_2 = bootstrapped_curves[system_sizes[i+1]][k, :]
        x_data = [job.sp.z for job in sorted(jobs, key=lambda job: job.sp.z)]
        
        z_min_index = x_data.index(z_min)
        z_max_index = x_data.index(z_max)

        y_A_1 = y_data_1[z_min_index]
        y_A_2 = y_data_1[z_max_index]
        y_B_1 = y_data_2[z_min_index]
        y_B_2 = y_data_2[z_max_index]

        mA = (y_A_2 - y_A_1)/(z_max-z_min)
        mB = (y_B_2 - y_B_1)/(z_max-z_min)

        root = (y_B_1 - y_A_1 + z_min*(mA - mB))/(mA-mB)
        
        int_pts = np.append(int_pts, root)
    errors[k] = int_pts

stds = errors.std(axis=0)

print(stds)
print(int_pts)
popt, pcov = curve_fit(crossing_fit, (1/L_param), int_pts, sigma=stds, absolute_sigma=True)
perr = np.sqrt(np.diag(pcov))
print("Critical Point:", popt[0])
print("Critical Exponent:", popt[2])
print("Error in Critical Point:", perr[0])

plt.errorbar(1/(L_param), int_pts, yerr=stds, fmt='o', label='Data')

x_smooth = np.linspace(min(1/L_param), max(1/L_param), 200)
# Convert these x-values back to L to feed into your model function
L_smooth = 1 / x_smooth 
# Plot the dense arrays
plt.plot(x_smooth, crossing_fit(L_smooth, *popt), 'r-', label='Weighted Fit')

plt.ylim(0.66, z_max)
plt.xlim(0, 0.15)

plt.xlabel('1/L')
plt.ylabel('Crossing Point z(L)')
plt.legend()
plt.show()
plt.savefig('abc.png')