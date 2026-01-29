import numpy as np
import matplotlib.pyplot as plt
import argparse
import arch_lattices
import os

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

    parser.add_argument('--action', required=True)
    parser.add_argument('directories', nargs='+')

    args = parser.parse_args()

    file = "src/lattice/adj-lists/adj_list_" + str(args.L) + "_" + args.lattice + ".txt"

    if not os.path.exists(file):
        lattice_graph = arch_lattices.gen_lattice(args.lattice, [args.L, args.L], True)
        
            
        with open(file, "w") as f:           # write lattice graph adjacency data into file to be read by main.cpp
            for x in lattice_graph.adjacency_list():
                print(x, file=f)
    
