import matplotlib.pyplot as plt
from netket.graph import Lattice
import networkx as nx
import arch_lattices
import os


L = 0
l = ""

with open("temp_lattice_parameters.txt") as f:                  # get equilibriated system's nodes from main.cpp file, to be handled by NetworkX to display network graph
    L = int(f.readline().strip())
    l = f.readline().strip() 

lattice_graph = arch_lattices.gen_lattice(l, [L , L])

os.remove("temp_lattice_parameters.txt")

graph = lattice_graph.to_networkx()                     # converting original NetKet lattice graph to NetworkX graph for coloring and display

nodes = []                                              # 1D vector that stores species/vacancy identities of each node
node_color = []                                        # nodes vector translated to a vector storing colors of each node, to be represented in NetworkX network graph

with open("node_color_data.txt") as f:                  # get equilibriated system's nodes from main.cpp file, to be handled by NetworkX to display network graph
    for x in f:
        nodes.append(int(x))

colors = [
    "white", "red", "blue", "green", "yellow",          # 1–5
    "orange", "purple", "pink", "brown", "gray",        # 6–10
    "cyan", "magenta", "lime", "navy", "olive",         # 11–15
    "teal", "maroon", "gold", "silver", "turquoise",    # 16–20
    "indigo", "violet", "coral", "beige", "salmon",     # 21–25
    "khaki", "lavender", "plum", "chocolate", "crimson",# 26–30
    "orchid", "darkgreen", "darkblue", "darkred", "darkorange",  # 31–35
    "darkviolet", "skyblue", "springgreen", "forestgreen", "peru",# 36–40
    "slateblue", "slategray", "tan", "thistle", "tomato",         # 41–45
    "wheat", "steelblue", "seagreen", "aquamarine", "black"       # 46–50
]

for k in range(nodes):
    n = nodes[k] 
    node_color.append(colors[n])

pos = nx.planar_layout(graph)
nx.draw(graph, pos, node_color=node_colors, with_labels=True)

plt.savefig('lattice_graph.png')

# diagnostics/tests

    