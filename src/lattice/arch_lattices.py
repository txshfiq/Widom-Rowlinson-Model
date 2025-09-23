from netket.graph import Lattice
import numpy as np

def gen_square_lattice(extent, arg):
    basis = np.array([
        [1.0, 0.0],
        [0.0, 1.0],
    ])

    g = Lattice(basis_vectors=basis, extent=extent, pbc=arg)
    return g

def gen_triangular_lattice(extent, arg):
    basis = np.array([
        [1.0, 0.0],
        [0.5, np.sqrt(3)/2],
    ])
    g = Lattice(basis_vectors=basis, extent=extent, pbc=arg)
    return g

def gen_hexagonal_lattice(extent, arg):
    basis = np.array([
        [1.0, 0.0],
        [0.5, np.sqrt(3)/2],
    ])

    cell = np.array([
        [0, 0],
        [0.5, np.sqrt(3)/6],
    ])

    g = Lattice(basis_vectors=basis, site_offsets=cell, extent=extent, pbc=arg)
    return g

def gen_kagome_lattice(extent, arg):
    basis = np.array([
        [1.0, 0.0],
        [0.5, np.sqrt(3)/2],
    ])
    cell = np.array([
        basis[0] / 2.0,
        basis[1] / 2.0,
        (basis[0]+basis[1])/2.0
    ])

    g = Lattice(basis_vectors=basis, site_offsets=cell, extent=extent, pbc=arg)
    return g

def gen_leaf_lattice(extent, arg):
    sq3 = np.sqrt(3)
    basis = np.array([
        [1.0, 0.0],
        [0.5, sq3/2],
    ])
    cell = np.array([
        [5.0/14, sq3/14],
        [5.0/7, sq3/7],
        [15.0/14, 3*sq3/14],
        [8.0/7, 3*sq3/7],
        [11.0/14, 5*sq3/14],
        [3.0/7, 2*sq3/7],
    ])

    g = Lattice(basis_vectors=basis, site_offsets=cell, extent=extent, pbc=arg)
    return g

def gen_ruby_lattice(extent, arg):
    sq3 = np.sqrt(3)
    a = 1 / (1+sq3)

    basis = np.array([
        [1.0, 0.0],
        [0.5, sq3/2],
    ])
    cell = np.array([
        [a*sq3/2,a/2],
        [(1+sq3/2)*a,a/2],
        [1/2,(1+sq3)/2*a],
        [1,a],
        [(3-(2+sq3)*a)/2,(sq3-a)/2],
        [(3 - sq3 * a)/2,(sq3 - a)/2]
    ])

    g = Lattice(basis_vectors=basis, site_offsets=cell, extent=extent, pbc=arg)
    return g

def gen_star_lattice(extent, arg):
    sq3 = np.sqrt(3)
    a = 1 / (2+sq3)

    basis = np.array([
        [1.0, 0.0],
        [0.5, sq3/2],
    ])
    cell = np.array([
        [1/2,a/2],
        [(1-a)/2, (1+sq3)/2*a],
        [(1+a)/2, (1+sq3)/2*a],
        [1, (sq3-a)/2],
        [1-a/2,(sq3-(1+sq3)*a)/2],
        [1+a/2,(sq3-(1+sq3)*a)/2]
    ])

    g = Lattice(basis_vectors=basis, site_offsets=cell, extent=extent, pbc=arg)
    return g

def gen_SHD_lattice(extent, arg):
    sq3 = np.sqrt(3)
    a = 2 / (6+2*sq3)

    basis = np.array([
        [1.0, 0.0],
        [0.5, sq3/2],
    ])
    hex1 = np.array([
        [(1+sq3/2)*a,a/2],
        [(2+sq3/2)*a,a/2],
        [(1+sq3/2)*a,(1/2 + sq3)*a],
        [(2+sq3/2)*a,(1/2 + sq3)*a],
        [(1+sq3)/2*a,(1+sq3)*a/2],
        [(5+sq3)/2*a,(1+sq3)*a/2],
    ])
    hex2 = hex1 + np.array([sq3/2,1/2])*(1+sq3)*a
    cell = np.vstack((hex1,hex2))

    g = Lattice(basis_vectors=basis, site_offsets=cell, extent=extent, pbc=arg)
    return g

def gen_trellis_lattice(extent, arg): 
    sq3 = np.sqrt(3)
    a = 1

    basis = np.array([
        [1+(sq3/2), 0.5],
        [0.0, 1.0],
    ])

    cell = np.array([
        [0.5, 0.5],
        [(1.0+sq3)/2.0 , 1],
    ])

    g = Lattice(basis_vectors=basis, site_offsets=cell, extent=extent, pbc=arg)
    return g

def gen_bathroom_lattice(extent, arg):
    a = 1.0/(1.0+np.sqrt(2))

    basis = np.array([
        [1, 0],
        [0, 1],
    ])
    
    cell = np.array([
        [a/2 , 1.0/2.0],
        [0.5 , a/2],
        [1.0-(a/2) , 0.5],
        [0.5 , 1.0-(a/2)],
    ])
    
    
    g = Lattice(basis_vectors=basis, site_offsets=cell, extent=extent, pbc=arg)
    return g

def gen_snub_lattice(extent, arg):
    sq3 = np.sqrt(3)
    sq2 = np.sqrt(2)
    sq6 = np.sqrt(6)
    a = 1.0/np.sqrt(2.0+sq3)

    ones = np.ones(2)

    basis = np.array([
        [1.0, 0.0],
        [0.0, 1.0],
    ])

    cell = np.array([
        [a*np.sqrt((7.0/8.0)+(sq3/2.0)) , a*sq2/4.0],
        [a*sq2/4.0 , a*sq6/4.0],
        ones - [a*np.sqrt((7.0/8.0)+(sq3/2.0)) , a*sq2/4.0],
        ones - [a*sq2/4.0 , a*sq6/4.0],
    ])

    g = Lattice(basis_vectors=basis, site_offsets=cell, extent=extent, pbc=arg)
    return g

def gen_lattice(lattice, extent, arg):
    if lattice == "square":
        return gen_square_lattice(extent, arg)
    elif lattice == "triangular":                    
        return gen_triangular_lattice(extent, arg)
    elif lattice == "hexagonal":
        return gen_hexagonal_lattice(extent, arg)
    elif lattice == "kagome":
        return gen_kagome_lattice(extent, arg)
    elif lattice == "leaf":
        return gen_leaf_lattice(extent, arg)
    elif lattice == "ruby":
        return gen_ruby_lattice(extent, arg)
    elif lattice == "star":
        return gen_star_lattice(extent, arg)
    elif lattice == "SHD":
        return gen_SHD_lattice(extent, arg)
    elif lattice == "trellis":
        return gen_trellis_lattice(extent, arg)
    elif lattice == "bathroom":
        return gen_bathroom_lattice(extent, arg)
    elif lattice == "snub":
        return gen_snub_lattice(extent, arg)
    else:
        raise ValueError(f"Invalid lattice type: {lattice}")

