import numpy as np
import matplotlib.pyplot as plt
import argparse
import arch_lattices

if __name__ == '__main__':

    parser = argparse.ArgumentParser()

    parser.add_argument(
        "-L", "--L",
        type=int,
        required=True,
        help="Dimensional quantity for number of unit cells (basically lattice size L)"
    )
    parser.add_argument(
        "-l", "--lattice",
        type=str,
        required=True,
        help="Lattice type"
    )

    args = parser.parse_args()

    lattice_graph = arch_lattices.gen_lattice(args.lattice, [args.L, args.L])
        
    with open("temp_lattice_data.txt", "w") as f:           # write lattice graph adjacency data into file to be read by main.cpp
        for x in lattice_graph.adjacency_list():
            print(x, file=f)

    with open("temp_lattice_parameters.txt", "w") as f:
        print(args.L)
        print(args.lattice)