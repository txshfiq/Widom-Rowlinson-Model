import signac
import numpy as np
import os
from tqdm import tqdm
from scipy.optimize import curve_fit
import matplotlib.pyplot as plt
from scipy.interpolate import CubicSpline
import glob


def binder_cumulant(data):
    data = np.array(data)
    s_2 = data**2
    s_4 = data**4

    U_L = 1 - (1/3)*(np.mean(s_4)/np.mean(s_2)**2)

    return U_L


def crossing_fit(L, zc, A):
    return zc + A * (1/(L))

param = "demixed"        # type of order parameter
N = 1500                 # number of bootstrap samples
burn_in = 10000           # number of initial samples to discard for equilibration
cubic_spline = True     # whether to use cubic spline interpolation for root finding
fixed_window = False      # whether to use a fixed window for root finding

files = glob.glob(os.path.join("/home/tashfiq/wr_lattice/src/actions/bootstrap_graphs", "*"))
for f in files:
    if os.path.isfile(f): 
        os.remove(f)


if __name__ == '__main__':

    project = signac.get_project("/home/tashfiq/wr_lattice/data/workspace")

    container = {}
    loader = tqdm(project)

    for job in loader:
        loader.set_description(f"Loading trajectory {job.id}")

        L = job.sp.L
        M = job.sp.M
        z = job.sp.z
        lat = job.sp.lat
        run = job.sp.run

        data = []

        filename = param + "_L" + str(L) + "_M" + str(M) + "_z" + f"{z:.3f}".replace('.', '-') + "_" + lat + "_run" + str(run) + ".txt"
        pathname = "/home/tashfiq/wr_lattice/data/sampling/" + param + "/" + filename

        if not os.path.exists(pathname):
            continue

        data = np.loadtxt(pathname)

        data = data[burn_in:]

        U_L = binder_cumulant(data)

        container.setdefault(L, {}).setdefault(z, {})[run] = U_L

    for L, z_map in container.items():
        for z, runs in z_map.items():
            run_arr = np.array(list(runs.values()))
            avgs = []
            for _ in range(N):
                sample = np.random.choice(run_arr, size=len(run_arr), replace=True)
                ensemble_avg = np.mean(sample)
                avgs.append(ensemble_avg)
            
            runs['avgs'] = avgs

    sorted_container = {k: dict(sorted(v.items())) for k, v in sorted(container.items())}

    graphs_dir = os.path.join(os.path.dirname(__file__), "bootstrap_graphs")
    os.makedirs(graphs_dir, exist_ok=True)

    critical_points = []
    int_pts = np.array([])
    L_finite = np.array(list(sorted_container.keys())[:-1])

    more_than_one_root = False
    no_root_found = False


    batches = tqdm(range(N))

    for i in batches:
        batches.set_description(f"Processing bootstrap samples")
        int_pts_i = np.array([])
        prev_L = None

        for L, z_map in sorted_container.items():
            
            if fixed_window:
                min_z = min(z_map.keys())
                max_z = max(z_map.keys())
            else:
                min_z = -100000
                max_z = 100000

            if prev_L is None:
                prev_L = L
                continue
            
            prev_line = []
            line = []
            for z, runs in z_map.items():
                prev_cumulant = sorted_container[prev_L][z]['avgs'][i]
                cumulant = runs['avgs'][i]
                prev_line.append(prev_cumulant)
                line.append(cumulant)

            y_diff = np.array(prev_line) - np.array(line)
            
            if cubic_spline:
                cs = CubicSpline(list(z_map.keys()), y_diff, bc_type='natural')
                roots = cs.roots()
                all_roots = roots[np.isreal(roots)].real
                all_roots = all_roots[(all_roots >= min_z) & (all_roots <= max_z)]
            else:
                coeffs = np.polyfit(list(z_map.keys()), y_diff, 3)
                roots = np.roots(coeffs)
                all_roots = roots[np.isreal(roots)].real
                all_roots = all_roots[(all_roots >= min_z) & (all_roots <= max_z)]

            guess = 4.46

            if len(all_roots) == 0:
                no_root_found = True
                all_roots = guess
                int_pts_i = np.append(int_pts_i, all_roots)  
            elif len(all_roots) > 1:
                more_than_one_root = True
                correct = np.argmin(np.abs(all_roots - guess))
                all_roots = all_roots[correct].item()
                int_pts_i = np.append(int_pts_i, all_roots)
            else:
                int_pts_i = np.append(int_pts_i, all_roots.item())

            prev_L = L
        
        int_pts = np.vstack([int_pts, int_pts_i]) if int_pts.size else int_pts_i

        # Plot cumulant U_L vs activity (z) for this bootstrap sample.
        plt.figure()
        for L, z_map in sorted_container.items():
            zs = np.array(sorted(z_map.keys()))
            u_values = np.array([z_map[z]['avgs'][i] for z in zs])
            plt.plot(zs, u_values, marker='o', linestyle='-', label=f'L={L}')

        plt.xlabel('Activity $z$')
        plt.ylabel(r'Cumulant $U_L$')
        plt.title(f'Bootstrap sample {i+1}: $U_L$ vs activity')
        plt.legend(fontsize='small', loc='best')
        plt.tight_layout()
        sample_path = os.path.join(graphs_dir, f'bootstrap_sample_{i+1:04d}.png')
        plt.savefig(sample_path, dpi=300, bbox_inches='tight')
        plt.close()

        '''
        print()
        print()
        print(f"Bootstrap sample {i+1}: {int_pts_i}")
        print(f"Lattice sizes: {L_finite}")
        '''

        popt, pcov = curve_fit(crossing_fit, L_finite, int_pts_i)
        critical_points.append(popt[0])
    critical_points = np.array(critical_points)
    print(f"Critical point: {np.mean(critical_points)} ± {np.std(critical_points)}")

    points = np.mean(int_pts, axis=0)
    errors = np.std(int_pts, axis=0)

    plt.errorbar(1/L_finite, points, yerr=errors, fmt='o', label='Data')

    x_smooth = np.linspace(1e-6, max(1/L_finite), 200)
    L_smooth = 1 / x_smooth 
    plt.plot(x_smooth, crossing_fit(L_smooth, *popt), 'r-', label='Fitted Curve')

    plt.ylim()

    plt.xlabel(r'$L^{-1}$')
    plt.ylabel(r'Crossing Point $z_c(L)$')
    plt.legend()
    plt.savefig('finite-size_scaling.png', dpi=300, bbox_inches='tight')
    
    plt.figure()
    plt.hist(critical_points, bins=20, color='skyblue', edgecolor='black')
    plt.title('Distribution of Critical Points from Bootstrap Samples')
    plt.xlabel('Critical Point $z_c$')
    plt.ylabel('Frequency')
    plt.savefig('zc_distribution.png', dpi=300, bbox_inches='tight')

    if more_than_one_root:
        print("Warning: More than one intersection was found for some bootstrap samples.")
    if no_root_found:
        print("Warning: No intersection was found for some bootstrap samples.")



    
    


            

                
            

            