#include <openrand/philox.h>
#include <argparse/argparse.hpp>
#include <bits/stdc++.h>
#include <complex>
#include <algorithm>
#include <filesystem>


using namespace std;

// M = # of species
// L = lattice size (L x L)
// z = fugacity (absolute activity) -> constant value, same chemical potential throughout (grand-canonical ensemble)

struct MyArgs : public argparse::Args {
    double &z                    = kwarg("z", "Fugacity (absolute activity) value");
    int &L                        = kwarg("L", "Lattice size (L x L)");
    int &M                        = kwarg("M", "Number of species");
    string &lat                    = kwarg("lat", "Lattice Type");
};

/* PUT THIS INTO COMMAND LINE (assuming you are in the parent directory as this file)

    g++ -std=c++17 -I./include src/main.cpp -o main -lstdc++fs -O3
    ./main --L 24 --M 5 --z 3.6 --lat square

*/

// seeding random number generator (Philox)

std::random_device rd;
uint64_t seed = (static_cast<uint64_t>(rd()) << 32) | static_cast<uint64_t>(rd());
openrand::Philox rng(seed, 0);

int roundDownToNearestTen(double value) {
    return std::floor(value / 10.0)*10.0;
}

/////////////

/*

* Citation:
* Wicaksono, J. K. (2025). Greedy vs Backtracking: A comparative study of
* graph vertex coloring algorithms with C++ implementations. Makalah
* IF1220 Matematika Diskrit, Institut Teknologi Bandung.

*/

using Graph = vector<vector<int>>;

bool isSafe(int v, int c, const Graph &G, const vector<int> &color) {
    for (int u : G[v]) {
        if (color[u] == c)
            return false;
    }
    return true;
}

bool colorVertex(int v, const Graph &G, vector<int> &color, int k, int n) {
    if (v == n)
        return true;

    for (int c = 1; c <= k; ++c) {
        if (isSafe(v, c, G, color)) {
            color[v] = c;
            if (colorVertex(v + 1, G, color, k, n)) {
                return true;
            }
            color[v] = 0; // Backtrack
        }
    }
    return false;
}

vector<int> backtrackGraphColoring(const Graph &G, int k, int n) {
    vector<int> color(n, 0); // 0-based indexing

    if (colorVertex(0, G, color, k, n)) {
        return color;
    } else {
        return {}; // No valid coloring
    }
}

bool isBipartite(const std::vector<std::vector<int>>& adj) {
    int n = adj.size();
    std::vector<int> color(n, -1);  // -1 = uncolored, 0 and 1 are the two colors

    for (int start = 0; start < n; ++start) {
        if (color[start] != -1) continue;  // already visited in another component

        // BFS from this component
        std::queue<int> q;
        color[start] = 0;
        q.push(start);

        while (!q.empty()) {
            int u = q.front(); q.pop();
            for (int v : adj[u]) {
                if (color[v] == -1) {
                    // assign opposite color to neighbor
                    color[v] = color[u] ^ 1;
                    q.push(v);
                }
                else if (color[v] == color[u]) {
                    // found same-color neighbor → not bipartite
                    return false;
                }
            }
        }
    }

    return true;
}

std::vector<int> clusterFinder(const std::vector<int>& nodes, const std::vector<std::vector<int>>& adj, int start) {

    std::vector<bool> visited(nodes.size(), false);
    std::queue<int> q;
    std::vector<int> cluster_vertices;

    const int target_value = nodes[start];

    visited[start] = true;
    q.push(start);
    cluster_vertices.push_back(start);

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        for (int v : adj[u]) {
            if (!visited[v] && nodes[v] == target_value) {
                visited[v] = true;
                q.push(v);
                cluster_vertices.push_back(v);
            }
        }
    }

    return cluster_vertices;
}

std::vector<double> sublattice_densities(std::vector<int> nodes, std::vector<int> sublattice_locations) {
    int k = *std::max_element(sublattice_locations.begin(), sublattice_locations.end()); // size of rho array

    vector<double> arr;
    
    for (int i = 1; i <= k; i++) {
        int sub_tot = std::count(sublattice_locations.begin(), sublattice_locations.end(), i);
        double rho = 0;
        for (int j = 0; j < nodes.size(); j++) {
            if (sublattice_locations[j] == i && nodes[j] != 0) {
                rho++;
            }
        }
        rho /= sub_tot;
        arr.push_back(rho);
    }

    return arr;

}

double stdev(std::vector<double> arr, int n) {
    double sum = 0.0;
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }
    double mean = sum / n;

    double sqSum = 0.0;
    for (int i = 0; i < n; i++) {
        double diff = arr[i] - mean;
        sqSum += diff * diff;
    }

    return std::sqrt(sqSum / (n));
}

double crystalParameterTest(std::vector<int> nodes, std::vector<int> sublattice_locations) {
    std::vector<double> arr = sublattice_densities(nodes, sublattice_locations);
    int k = arr.size();

    return (k / std::sqrt(k-1)) * stdev(arr, k);
}

double crystalParameter(std::vector<int> nodes, std::vector<int> sublattice_locations) {
    /*
    double c = 0;
    for (int i = 0; i < nodes.size(); i++) {
        if (sublattice_locations[i] == 1) {
            if (nodes[i] != 0) {
                c++;
            }
            else {
                c--;
            }
        }
        if (sublattice_locations[i] == 2) {
            if (nodes[i] != 0) {
                c--;
            }
            else {
                c++;
            }
        }
    }
    c /= nodes.size();
    return std::abs(c);
    */

    std::vector<double> arr = sublattice_densities(nodes, sublattice_locations);
    int k = arr.size();

    return (k / std::sqrt(k-1)) * stdev(arr, k);

}

void visLattice(
    const std::vector<int>& node_values,
    const std::vector<std::vector<int>>& adjacency_list
) {
    int n_nodes = node_values.size();
    int N = static_cast<int>(std::sqrt(n_nodes));

    if (N * N != n_nodes) {
        throw std::runtime_error("Node count is not a perfect square.");
    }

    if (adjacency_list.size() != node_values.size()) {
        throw std::runtime_error("Adjacency list size mismatch.");
    }

    auto color_code = [](int v) -> std::string {
        switch (v) {
            case 0:  return "\033[40m";   // black
            case 1:  return "\033[41m";   // red
            case 2:  return "\033[42m";   // green
            case 3:  return "\033[43m";   // yellow
            case 4:  return "\033[44m";   // blue
            case 5:  return "\033[45m";   // magenta
            case 6:  return "\033[46m";   // cyan
            case 7:  return "\033[47m";   // white
            case 8:  return "\033[100m";  // bright black (gray)
            case 9:  return "\033[101m";  // bright red
            case 10: return "\033[102m";  // bright green
            case 11: return "\033[103m";  // bright yellow
            case 12: return "\033[104m";  // bright blue
            case 13: return "\033[105m";  // bright magenta
            case 14: return "\033[106m";  // bright cyan
            case 15: return "\033[107m";  // bright white
            default: throw std::runtime_error("Invalid node value (must be 0–15)");
        }
    };

    const std::string reset = "\033[0m";

    for (int r = 0; r < N; r++) {
        for (int c = 0; c < N; c++) {
            int idx = r * N + c;
            std::cout << color_code(node_values[idx]) << "  " << reset;
        }
        std::cout << "\n";
    }
}


double density(std::vector<int> nodes) {
    int count = 0;
    for (int n : nodes) {
        if (n != 0) {
            count++;
        }
    }
    return double(count) / nodes.size();
}

double demixedParameter(std::vector<int> nodes, int M) {
   std::complex<double> total = std::complex<double>(0, 0);
    for (int i = 1; i <= M; i++) {
        double angle = 2 * M_PI * (i - 1)/M;
        std::complex<double> euler = std::exp(std::complex<double>(0, -1 * angle));
        auto N_i = std::count(nodes.begin(), nodes.end(), i);
        double m_i = static_cast<double>(N_i) / (density(nodes) * nodes.size());
        total += (m_i * euler);
    }
    return std::abs(total);
}

int randInt(int x, int y) {
    std::uniform_int_distribution<int> dist(x, y);
    int a = dist(rng);  // y ∼ Uniform{a,…,b}   

    return a;
}

int randIntWithoutVal(int x, int y, int val) {
    std::uniform_int_distribution<int> dist(x, y);
    int a = dist(rng);  // y ∼ Uniform{a,…,b}   

    while (a == val) {
        a = dist(rng);
    }
    return a;
}

struct Color {
    int r, g, b;
};

void generateLatticeImage(const std::vector<int>& nodes, const std::vector<std::vector<int>>& adj, const std::string& filename) {
    if (nodes.empty()) {
        std::cerr << "Error: Node vector is empty." << std::endl;
        return;
    }

    int N = nodes.size();
    
    // Explicitly calculate square lattice dimensions based on vector size
    int cols = std::round(std::sqrt(N));
    int rows = (N + cols - 1) / cols;

    // Double-check to ensure it is actually a perfect square lattice
    if (cols * rows != N) {
        // Fallback: If it's a rectangle, recalculate rows cleanly
        rows = N / cols;
    }

    // 15-color palette (0 = Black, 1-15 = Distinct Colors)
    std::vector<Color> palette = {
        {0, 0, 0},       // 0: Black
        {255, 0, 0},     // 1: Red
        {0, 255, 0},     // 2: Green
        {0, 0, 255},     // 3: Blue
        {255, 255, 0},   // 4: Yellow
        {0, 255, 255},   // 5: Cyan
        {255, 0, 255},   // 6: Magenta
        {255, 128, 0},   // 7: Orange
        {128, 0, 128},   // 8: Purple
        {0, 128, 128},   // 9: Teal
        {128, 128, 0},   // 10: Olive
        {255, 192, 203}, // 11: Pink
        {165, 42, 42},   // 12: Brown
        {128, 128, 128}, // 13: Gray
        {255, 215, 0},   // 14: Gold
        {0, 250, 154}    // 15: Medium Spring Green
    };

    int cellSize = 10; // 30x30 pixels per grid block
    int width = cols * cellSize;
    int height = rows * cellSize;

    std::string outFilename = filename;
    if (outFilename.find(".ppm") == std::string::npos) {
        outFilename += ".ppm";
    }

    std::ofstream out(outFilename);
    if (!out) {
        std::cerr << "Error: Could not open file " << outFilename << " for writing." << std::endl;
        return;
    }

    // Write PPM Header
    out << "P3\n" << width << " " << height << "\n255\n";

    // Generate the pixels row by row
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            
            // Map pixel coordinate back to the 2D grid cell
            int gridX = x / cellSize;
            int gridY = y / cellSize;
            
            // Map 2D grid cell to the 1D nodes vector index
            int nodeIndex = gridY * cols + gridX;

            Color c = {255, 255, 255}; // Default background (White)

            if (nodeIndex < N) {
                int val = nodes[nodeIndex];
                if (val >= 0 && val <= 15) {
                    c = palette[val];
                } else {
                    c = palette[(val % 15) + 1]; // Handle values > 15 gracefully
                }
            }

            // Draw clean dark grid boundaries around cells
            if (x % cellSize == 0 || y % cellSize == 0) {
                c = {40, 40, 40}; 
            }

            out << c.r << " " << c.g << " " << c.b << " ";
        }
        out << "\n";
    }

    std::cout << "Successfully generated " << outFilename << " (" << width << "x" << height << " px) for a " << cols << "x" << rows << " lattice.\n";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
    MyArgs args = argparse::parse<MyArgs>(argc, argv);

    int L = args.L;
    int M = args.M;
    double z = args.z;
    string lat = args.lat;
    
    int sweeps = 6500000; // will be modified during runtime when equilibrium point is reached

    int k = 0; // number of colors (or sublattices) in lattice graph, will be set to 2 or 3 depending on the k-partiteness of the lattice

    if (L <= 0 || M <= 0 || z <= 0) {
        std::cerr << "Error: All parameters must be positive values." << std::endl;
        return 1;
    }

    std::ostringstream oss;
    oss.str("");
    oss << std::fixed << std::setprecision(3) << z;
    std::string str_z = oss.str();
    std::string str_z_noformat = str_z; 
    
    for (char &c : str_z) {
        if (c == '.') {
            c = '-';
        }
    }

    std::string cp_filename = "data/sampling/crystal/crystal_L" + std::to_string(L) + "_M" + std::to_string(M) + "_z" + str_z + "_" + args.lat + ".txt";
    std::string dp_filename = "data/sampling/demixed/demixed_L" + std::to_string(L) + "_M" + std::to_string(M) + "_z" + str_z + "_" + args.lat + ".txt";
    std::string de_filename = "data/sampling/density/density_L" + std::to_string(L) + "_M" + std::to_string(M) + "_z" + str_z + "_" + args.lat + ".txt";

    // opening data files
    std::ofstream cp_data(cp_filename.c_str());
    std::ofstream dp_data(dp_filename.c_str());
    std::ofstream de_data(de_filename.c_str());
    
    std::vector<std::vector<int>> lattice_adjacency_list;

    std::string adj_data_file = "src/lattice/adj-lists/adj_list_" + std::to_string(L) + "_" + args.lat + ".txt";
    std::ifstream file(adj_data_file);

    if (!file) {
        std::cerr << "Missing adjacency list: " << adj_data_file
                  << " (run Row action `generate_lattice` first)" << std::endl;
        return 1;
    }

    string line;

    while (std::getline(file, line)) {
        // turn “[”, “]”, “,” into plain spaces:
        for (char& c : line) {
            if (c=='[' || c==']' || c==',') c = ' ';
        }

        std::istringstream iss(line);
        std::vector<int> adj_list_push_back;

        int neighbor;
        while (iss >> neighbor) {
            adj_list_push_back.push_back(neighbor);
        }

        lattice_adjacency_list.push_back(adj_list_push_back);
    }


    if (isBipartite(lattice_adjacency_list)) {
        k = 2;
    } else {
        k = 3;
    }

    std::vector<int> nodes(lattice_adjacency_list.size(), 0);
    std::vector<int> sublattice_locations = backtrackGraphColoring(lattice_adjacency_list, k, lattice_adjacency_list.size());

    std::bernoulli_distribution bernoulli_trial((M*z)/((M*z)+1));
    
    for (int i = 0; i < nodes.size(); i++) {
        bool success = bernoulli_trial(rng);
        if (success == false) { // if bernoulli probability outcomes false, make lattice(i,j) empty (0)
            continue; // move to next iteration
        }
        else {
            int k = randInt(1, M); // generate species (k = 1, 2, 3, ... , M)
            bool conflict = false;
            for (int j = 0; j < lattice_adjacency_list[i].size(); j++) {
                int index = lattice_adjacency_list[i][j];
                if (k != nodes[index] && nodes[index] != 0) {
                    conflict = true;
                    break;
                }
            }
            if (conflict == false) {
                nodes[i] = k;
            }
            else {
                nodes[i] = 0;
            }
        }
    }
    
    int s = 1; // start at sweep 1
    int c = 1;

    double p = 0.95;

    std::bernoulli_distribution p_remove(p); // for cluster flipping
    std::bernoulli_distribution A_remove(std::min(1.0, (1.0/(z*M*p))));
    std::bernoulli_distribution A_insert(std::min(1.0, (z*M*p)));

    while (s <= sweeps) {
        for (int m = 0; m < nodes.size(); m++) {
            int i = randInt(0, nodes.size()-1); // Choose a site at random
            int k = randInt(1, M);              // Choose a color at random

            if (nodes[i] != 0) {
                if (p_remove(rng)) {
                    if (A_remove(rng)) {
                        nodes[i] = 0;
                    }
                    else {
                        continue;
                    }
                }
                else {
                    std::vector<int> cluster = clusterFinder(nodes, lattice_adjacency_list, i);

                    int col = randIntWithoutVal(1, M, nodes[i]);
                    for (int v : cluster) { 
                        nodes[v] = col;
                    }
                }
            }
            else {
                if (A_insert(rng)) {
                    bool conflict = false;
                        for (int j = 0; j < lattice_adjacency_list[i].size(); j++) {
                            int index = lattice_adjacency_list[i][j];
                            if (k != nodes[index] && nodes[index] != 0) {
                                conflict = true;
                                break;
                            }
                        }

                    if (conflict == false) {
                        nodes[i] = k;
                    }
                    else {
                        continue;
                    }
                }
                else {
                    continue;
                }
            }

        }
        
        /*
        if (s % 100 == 0) {
            std::string folder = "data/movies/M" + std::to_string(M) + "/z" + str_z;

            // Create all missing directories in the path
            std::filesystem::create_directories(folder);

            std::string name = folder + "/" + std::to_string(c) + ".ppm";

            generateLatticeImage(
                nodes,
                lattice_adjacency_list,
                name
            );

            c++;
        }
        */

        double cp = crystalParameter(nodes, sublattice_locations);
        double de = density(nodes);
        double dp = demixedParameter(nodes, M);

        cp_data << cp << std::endl;
        dp_data << dp << std::endl;
        de_data << de << std::endl;

        s++;
    }

    cp_data.close();
    dp_data.close();
    de_data.close();  

    return 0;
}
