import signac
import argparse
import subprocess

def compile_wr(source: str, output: str):
    cmd = [
        'g++',
        '-std=c++17',
        '-I./include',            # include directory
        source,                   # e.g. 'src/wr_square.cpp'
        '-o', output,             # e.g. 'wr_square'
        '-lstdc++fs',             # link the filesystem library
    ]
    print("Compiling... lmfao")
    subprocess.run(cmd, check=True)

def run_wr(executable: str, L: int, M: int, z: float) -> str:
    cmd = [
        f'./{executable}',
        '--L', str(L),
        '--M', str(M),
        '--z', str(z),
    ]
    print("Running ts where L =", L, "M =", M, "z =", z)
    completed = subprocess.run(
        cmd,
        capture_output=True,
        text=True,
        check=True
    )
    return completed.stdout


def run_simulation(*job):
    cumulant = float(output)
    for job in project:
        with open(job.fn("cumulant_values.txt"), "w") as file:
            file.write(str(cumulant) + "\n")

if __name__ == '__main__':
    # Parse the command line arguments: python action.py --action <ACTION> [DIRECTORIES]
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
    parser.add_argument('--action', required=True)
    parser.add_argument('directories', nargs='+')

    args = parser.parse_args()

    # Open the signac jobs
    project = signac.get_project("/home/tashfiq/wr_lattice/data/workspace")
    jobs = [project.open_job(id=directory) for directory in args.directories]

    SRC  = 'src/wr_square.cpp'
    BIN  = 'wr_square'

    L = args.num3
    M = args.num2
    z = args.num1

    compile_wr(SRC, BIN)
    output = run_wr(BIN, L, M, z)
    print("Program output:\n", output)

    # Call the action
    # globals()[args.action](*jobs)
