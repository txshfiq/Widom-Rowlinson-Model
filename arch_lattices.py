from netket.graph import Lattice
import numpy as np

def gen_square_lattice(extent):
    basis = np.array([
        [1.0, 0.0],
        [0.0, 1.0],
    ])

    g = Lattice(basis_vectors=basis, extent=extent, pbc=True)
    return g

def gen_triangular_lattice(extent):
    basis = np.array([
        [1.0, 0.0],
        [0.5, np.sqrt(3)/2],
    ])
    g = Lattice(basis_vectors=basis, extent=extent, pbc=True)
    return g

def gen_hexagonal_lattice(extent):
    basis = np.array([
        [1.0, 0.0],
        [0.5, np.sqrt(3)/2],
    ])

    cell = np.array([
        [0, 0],
        [0.5, np.sqrt(3)/6],
    ])

    g = Lattice(basis_vectors=basis, site_offsets=cell, extent=extent, pbc=True)
    return g

def gen_kagome_lattice(extent):
    basis = np.array([
        [1.0, 0.0],
        [0.5, np.sqrt(3)/2],
    ])
    cell = np.array([
        basis[0] / 2.0,
        basis[1] / 2.0,
        (basis[0]+basis[1])/2.0
    ])

    g = Lattice(basis_vectors=basis, site_offsets=cell, extent=extent, pbc=True)
    return g

def gen_leaf_lattice(extent):
    basis = np.array([
        [1.0, 0.0],
        [0.5, np.sqrt(3)/2],
    ])
    cell = np.array([
        basis[0] / 2.0,
        basis[1] / 2.0,
        (basis[0]+basis[1])/6.0
    ])

    g = Lattice(basis_vectors=basis, site_offsets=cell, extent=extent, pbc=True)
    return g

def gen_lattice(lattice, extent):
    if lattice == "square":
        return gen_square_lattice(extent)
    elif lattice == "triangular":
        return gen_triangular_lattice(extent)
    elif lattice == "hexagonal":
        return gen_hexagonal_lattice(extent)
    elif lattice == "kagome":
        return gen_kagome_lattice(extent)
    elif lattice == "leaf":
        return gen_leaf_lattice(extent)
    else:
        raise ValueError(f"Invalid lattice type: {lattice}")

