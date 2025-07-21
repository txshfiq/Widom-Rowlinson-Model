from netket.graph import Lattice
import networkx as nx
import arch_lattices
import os
import plotly.graph_objects as go
import numpy as np
import argparse

def plot_network(G, pos, nodes, lat, L):
    """
    G = the network x object
    pos = [N, 2] vector of node positions
    nodes = identity of each node in lattice                    I ∈ [0, 1, 2, ... , M]
    """
    
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

    colorscale = 'Viridis'
    degrees = dict(G.degree())
    
    # Create a plotly Scatter plot for nodes
    node_x = []
    node_y = []
    node_color = []

    for node in G.nodes():
        x, y = pos[node]
        node_x.append(x)
        node_y.append(y)
        index = nodes[node]
        node_color.append(colors[index])

    node_trace = go.Scatter(
        x=node_x, y=node_y,
        mode='markers',
        marker=dict(
            size=10,
            color=node_color,
            colorscale=colorscale,
            colorbar=dict(
                title='Node Degree'
            ),
            showscale=True
        ),
        text=[f'Degree: {degrees[node]}' for node in G.nodes()],
        hoverinfo='text'
    )
    
    # Create a plotly Scatter plot for edges
    edge_x = []
    edge_y = []

    for edge in G.edges():
        x0, y0 = pos[edge[0]]
        x1, y1 = pos[edge[1]]
        edge_x.extend([x0, x1, None])
        edge_y.extend([y0, y1, None])

    edge_trace = go.Scatter(
        x=edge_x, y=edge_y,
        line=dict(width=0.5, color='#888'),
        hoverinfo='none',
        mode='lines')

    # Create figure
    fig = go.Figure(data=[edge_trace, node_trace],
                    layout=go.Layout(
                        title=f"Lattice Type: {lat}, L = {L}",
                        #titlefont_size=16,
                        showlegend=False,
                        hovermode='closest',
                        margin=dict(b=20, l=5, r=5, t=40),
                        xaxis=dict(showgrid=False, zeroline=False, showticklabels=False),
                        yaxis=dict(showgrid=False, zeroline=False, showticklabels=False))
                   )

    fig.show()

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

    lattice_graph = arch_lattices.gen_lattice(args.lattice, [args.L , args.L], False)

    graph = lattice_graph.to_networkx()                     # converting original NetKet lattice graph to NetworkX graph for coloring and display
    nodes = []                                              # 1D vector that stores species/vacancy identities of each node

    with open("node_color_data.txt") as f:                  # get equilibriated system's nodes from main.cpp file, to be handled by NetworkX to display network graph
        for x in f:
            nodes.append(int(x))

    plot_network(graph, lattice_graph.positions, nodes, args.lattice, args.L)

    # diagnostics/tests

    