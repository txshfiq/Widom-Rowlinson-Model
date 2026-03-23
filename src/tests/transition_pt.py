import numpy as np
import pandas as pd
import signac 
import seaborn as sns
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit
from scipy.interpolate import CubicSpline


def crossing_fit(L, zc, A):
    return zc + A * (1/(L))

start = 30      
step = 4       
length = 16        

system_sizes = [start + i*step for i in range(length)]


print(system_sizes)

project = signac.get_project("/home/tashfiq/wr_lattice/data/workspace")
L_param = np.array(system_sizes[:-1])
M = 3 ##############
n_bootstraps = 1500
x_data = [job.sp.z for job in sorted(project.find_jobs({'M': M, 'L': system_sizes[0]}), key=lambda job: job.sp.z)]
min_z = x_data[0] ##############
max_z = x_data[-1] ##############

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

    # cs = CubicSpline(x_data, y_diff, bc_type='natural')
    # all_roots = cs.roots()
    
    coeffs = np.polyfit(x_data, y_diff, 3)
    roots = np.roots(coeffs)
    all_roots = roots[np.isreal(roots)].real
    
    all_roots = all_roots[(all_roots >= min_z) & (all_roots <= max_z)]
    
    if len(all_roots) == 0:
        print("No intersection was found for finding actual intersection:", system_sizes[i], system_sizes[i+1])
        all_roots = int_pts_i.mean()
        int_pts_i = np.append(int_pts_i, all_roots)  
    elif len(all_roots) > 1:
        avg = int_pts_i.mean()
        correct = np.argmin(np.abs(all_roots - avg))
        all_roots = all_roots[correct].item()
        print("There were more than one intersections to find actual intersection:", system_sizes[i], system_sizes[i+1])
        int_pts_i = np.append(int_pts_i, all_roots)
    else:
        int_pts_i = np.append(int_pts_i, all_roots.item())

for k in range(n_bootstraps):
    int_pts = np.array([])
    
    for i in range(len(system_sizes)-1):
        y_data_1 = bootstrapped_curves[system_sizes[i]][k, :]
        y_data_2 = bootstrapped_curves[system_sizes[i+1]][k, :]

        y_diff = y_data_1 - y_data_2
        # cs = CubicSpline(x_data, y_diff, bc_type='natural')
        # all_roots = cs.roots()
        
        coeffs = np.polyfit(x_data, y_diff, 3)
        roots = np.roots(coeffs)
        all_roots = roots[np.isreal(roots)].real

        all_roots = all_roots[(all_roots >= min_z) & (all_roots <= max_z)]

        if len(all_roots) == 0:
            print("No intersection was found for finding error:", system_sizes[i], system_sizes[i+1])
            all_roots = int_pts.mean()
            int_pts = np.append(int_pts, all_roots)
        elif len(all_roots) > 1:
            avg = int_pts.mean()
            correct = np.argmin(np.abs(all_roots - avg))
            all_roots = all_roots[correct].item()
            print("There were more than one intersections to find error:", system_sizes[i], system_sizes[i+1])
            int_pts = np.append(int_pts, all_roots)
        else:
            int_pts = np.append(int_pts, all_roots.item())
    errors[k] = int_pts

stds = errors.std(axis=0)

p0 = [int_pts_i[-1], 1.0] 

popt, pcov = curve_fit(crossing_fit, L_param, int_pts_i, sigma=stds, absolute_sigma=True, p0=p0, maxfev=5000)

perr = np.sqrt(np.diag(pcov))
print("Critical Point", popt[0])
# print("Critical Exponent", popt[2])
print("Error in Critical Point", perr[0])

pred_y = crossing_fit(L_param, *popt)
srs = np.sum((int_pts_i - pred_y)**2)

print("Sum of Residuals Squared:", srs)
print("Mean Squared Error:", srs/len(int_pts_i))

plt.errorbar(1/(L_param), int_pts_i, yerr=stds, fmt='o', label='Data')

x_smooth = np.linspace(1e-6, max(1/L_param), 200)
L_smooth = 1 / x_smooth 
plt.plot(x_smooth, crossing_fit(L_smooth, *popt), 'r-', label='Fitted Curve')


plt.xlim(0, 0.035)
plt.ylim(2.6, 2.7)
plt.xlabel('1/L')
plt.ylabel('Crossing Point z(L)')
plt.legend()
plt.savefig('finite-size_scaling.png', dpi=300, bbox_inches='tight')
plt.show()
