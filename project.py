import signac
import argparse
import subprocess

'''
def compile_wr(source: str, output: str):
    cmd = [
        'g++',
        '-std=c++17',
        '-I./include',            # include directory
        source,                   # e.g. 'src/wr_square.cpp'
        '-o', output,             # e.g. 'wr_square'
        '-lstdc++fs',             # link the filesystem library
    ]
    print("Compiling...")
    subprocess.run(cmd, check=True)
'''

def run_wr(executable: str, L: int, M: int, z: float, lat: str) -> str:
    cmd = [
        f'./{executable}',
        '--L', str(L),
        '--M', str(M),
        '--z', str(z),
        '--lat', lat
    ]
    print(f"Simulation parameters: L = {L}, M = {M}, z = {z}, lat = {lat}")
    
    # --- MODIFICATION START ---
    try:
        completed = subprocess.run(
            cmd,
            capture_output=True,
            text=True,
            check=True
        )
        return completed.stdout
    
    except subprocess.CalledProcessError as e:
        print("--- C++ EXECUTABLE FAILED ---")
        print(f"Exit Code: {e.returncode}")
        print(f"Stdout from C++:\n{e.stdout}")
        print(f"Stderr from C++:\n{e.stderr}") 
        print("-------------------------------")
        raise 
    # --- MODIFICATION END ---

'''
def run_simulation(*job):
    cumulant = float(output)
    for job in project:
        with open(job.fn("cumulant_values.txt"), "w") as file:
            file.write(str(cumulant) + "\n")
'''

def output_cumulant(job, output: str):
    cumulant = float(output)
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
    jobs = [project.open_job(id=directory) for directory in args.directories]

    SRC  = 'src/main.cpp'
    BIN  = 'main'

    L = args.num3
    M = args.num2
    z = args.num1
    lat = args.str1

    output = run_wr(BIN, L, M, z, lat)

    for job in project:
        if job.sp.L == L and job.sp.M == M and job.sp.z == z:
            output_cumulant(job, output)
    
    print("Program output:\n", output)

    # Call the action
    # globals()[args.action](*jobs)