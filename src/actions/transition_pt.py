import numpy as np
import pandas as pd
import signac 
import seaborn as sns
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit
from scipy.interpolate import CubicSpline

def crossing_fit(L, zc, A):
    return zc + A * 1/(L**(2))

project = signac.get_project("/home/tashfiq/wr_lattice/data/workspace")
system_sizes = [4, 8, 16, 32, 64]
L_param = np.array(system_sizes[:-1])
M = 2 ##############
n_bootstraps = 1500
x_data = [job.sp.z for job in sorted(project.find_jobs({'M': M, 'L': system_sizes[0]}), key=lambda job: job.sp.z)]
min_z = x_data[0] ##############
max_z = x_data[-1] ##############

k = 5  # Polynomial order for fitting


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
int_pts_i = np.array([])

for i in range(len(system_sizes)-1):
    y_data_1 = actual_cumulants[system_sizes[i]]
    y_data_2 = actual_cumulants[system_sizes[i+1]]

    y_diff = y_data_1 - y_data_2

    cs = CubicSpline(x_data, y_diff, bc_type='natural')

    all_roots = cs.roots()
    all_roots = all_roots[(all_roots >= min_z) & (all_roots <= max_z)]
    
    '''
    coeff_1 = np.polyfit(x_data, y_data_1, k)
    coeff_2 = np.polyfit(x_data, y_data_2, k)

    root_coeff = np.subtract(coeff_1, coeff_2)
    all_roots = np.roots(root_coeff)
    all_roots = all_roots[np.isreal(all_roots)].real


    all_roots = all_roots[(all_roots >= min_z) & (all_roots <= max_z)] ## USE THIS TO CONSTRAIN THE ROOTS
    '''
    if len(all_roots) == 0:
        error_msg = (
            f"Error: No intersection found between L={system_sizes[i]} and L={system_sizes[i+1]} "
            f"within the specified range."
        )
        raise ValueError(error_msg)

    if len(all_roots) > 1:
        error_msg = (
            f"Error: Ambiguous solution. Found {len(all_roots)} intersections "
            f"at x = {all_roots}. for lattice sizes: {system_sizes[i]} and {system_sizes[i+1]} Expected at most 1."
        )
        raise ValueError(error_msg)

    int_pts_i = np.append(int_pts_i, all_roots)

for k in range(n_bootstraps):
    int_pts = np.array([])
    
    for i in range(len(system_sizes)-1):
        y_data_1 = bootstrapped_curves[system_sizes[i]][k, :]
        y_data_2 = bootstrapped_curves[system_sizes[i+1]][k, :]

        y_diff = y_data_1 - y_data_2
        cs = CubicSpline(x_data, y_diff, bc_type='natural')

        all_roots = cs.roots()
        all_roots = all_roots[(all_roots >= min_z) & (all_roots <= max_z)]

        '''
        coeff_1 = np.polyfit(x_data, y_data_1, k)
        coeff_2 = np.polyfit(x_data, y_data_2, k)

        root_coeff = np.subtract(coeff_1, coeff_2)
        all_roots = np.roots(root_coeff)
        all_roots = all_roots[np.isreal(all_roots)].real


        all_roots = all_roots[(all_roots >= min_z) & (all_roots <= max_z)] ## USE THIS TO CONSTRAIN THE ROOTS
        '''

        if len(all_roots) == 0:
            error_msg = (
                f"Error: No intersection found between L={system_sizes[i]} and L={system_sizes[i+1]} "
                f"within the specified range."
            )
            raise ValueError(error_msg)

        if len(all_roots) > 1:
            error_msg = (
                f"Error: Ambiguous solution. Found {len(all_roots)} intersections "
                f"at x = {all_roots}. Expected at most 1."
            )
            raise ValueError(error_msg)
        
        int_pts = np.append(int_pts, all_roots)
    errors[k] = int_pts

stds = errors.std(axis=0)

p0 = [int_pts_i[-1], 1.0, 2]

popt, pcov = curve_fit(crossing_fit, L_param, int_pts_i, sigma=stds, absolute_sigma=True, maxfev=5000)
perr = np.sqrt(np.diag(pcov))
print("Critical Point", popt[0])
## print("Critical Exponent", popt[2])
print("Error in Critical Point", perr[0])

pred_y = crossing_fit(L_param, *popt)
srs = np.sum((int_pts_i - pred_y)**2)

print("Sum of Residuals Squared:", srs)

plt.errorbar(1/(L_param), int_pts_i, yerr=stds, fmt='o', label='Data')

x_smooth = np.linspace(min(1/L_param), max(1/L_param), 200)
# Convert these x-values back to L to feed into your model function
L_smooth = 1 / x_smooth 
# Plot the dense arrays
plt.plot(x_smooth, crossing_fit(L_smooth, *popt), 'r-', label='Weighted Fit')


plt.xlabel('1/L')
plt.ylabel('Crossing Point z(L)')
plt.legend()
plt.savefig('abc.png', dpi=300, bbox_inches='tight')
plt.show()
