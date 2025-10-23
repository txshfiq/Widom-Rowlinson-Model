#include <openrand/philox.h>
#include <argparse/argparse.hpp>
#include <bits/stdc++.h>
#include <stdexcept>
#include <complex>
#include <algorithm>
#include <unistd.h>
#include <termios.h>

using namespace std;


// M = # of species
// L = lattice size (L x L)
// z = fugacity (absolute activity) -> constant value, same chemical potential throughout (grand-canonical ensemble)


// Note for archimedean lattices:
// Triangular: L must be a multiple of 3


// Helper class for non-blocking keyboard input on Unix-like systems
class NonBlockingTerminal {
public:
    NonBlockingTerminal() {
        // Get current terminal settings
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        // Disable canonical mode (line buffering) and echo
        newt.c_lflag &= ~(ICANON | ECHO);
        // Apply the new settings
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    }

    ~NonBlockingTerminal() {
        // Restore the old terminal settings
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }

    // Check if a key has been pressed
    int kbhit() {
        struct timeval tv = { 0L, 0L };
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        return select(1, &fds, NULL, NULL, &tv);
    }

    // Read the character without blocking
    int getch() {
        int r;
        unsigned char c;
        if ((r = read(STDIN_FILENO, &c, sizeof(c))) < 0) {
            return r;
        } else {
            return c;
        }
    }

private:
    struct termios oldt, newt;
};

struct MyArgs : public argparse::Args {
    double &z                    = kwarg("z", "Fugacity (absolute activity) value");
    int &L                        = kwarg("L", "Lattice size (L x L)");
    int &M                        = kwarg("M", "Number of species");
    string &lat                    = kwarg("lat", "Lattice Type");
    int &sweeps                = kwarg("sweeps", "Number of sweeps (default: 2)");
};

/* PUT THIS INTO COMMAND LINE (assuming you are in the parent directory as this file)


    g++ -std=c++17 -O3 -I./include src/main_testing.cpp -o main_testing -lstdc++fs
    ./main_testing --L 20 --M 8 --z 0.8 --lat square --sweeps 50000


*/

// seeding random number generator (Philox)
std::random_device rd;
uint64_t seed = (static_cast<uint64_t>(rd()) << 32) | static_cast<uint64_t>(rd());
openrand::Philox rng(seed, 0);

int roundDownToNearestTen(double value) {
    return std::floor(value / 10.0)*10.0;
}


const char* colors[] = {
  "\033[0m",     // 0: reset (empty)
  "\033[31m",    // 1: red
  "\033[32m",    // 2: green
  "\033[33m",    // 3: yellow
  "\033[34m",    // 4: blue
  "\033[35m",    // 5: magenta
  "\033[36m",    // 6: cyan
  "\033[91m",    // 7: bright red
  "\033[92m",    // 8: bright green
  "\033[93m",    // 9: bright yellow
  "\033[94m",    // 10: bright blue
  "\033[95m",    // 11: bright magenta
  "\033[96m",    // 12: bright cyan
  "\033[90m",    // 13: gray
  "\033[97m",    // 14: white
  "\033[30m",    // 15: black
  "\033[1;31m",  // 16: bold red
  "\033[1;32m",  // 17: bold green
  "\033[1;33m",  // 18: bold yellow
  "\033[1;34m",  // 19: bold blue
  "\033[1;35m",  // 20: bold magenta
  "\033[1;36m",  // 21: bold cyan
  "\033[1;91m",  // 22: bold bright red
  "\033[1;92m",  // 23: bold bright green
  "\033[1;93m",  // 24: bold bright yellow
  "\033[1;94m",  // 25: bold bright blue
  "\033[1;95m",  // 26: bold bright magenta
  "\033[1;96m",  // 27: bold bright cyan
  "\033[1;97m",  // 28: bold white
  "\033[1;90m"   // 29: bold gray
};



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

void printLattice(std::vector<std::vector<int>>& arr, int M) {
    for (int i = 0; i < arr.size(); i++) {
        for (int j = 0; j < arr[i].size(); j++) {
        int s = arr[i][j];
        // pick a visual for the box—
        // here we use “■” (Unicode U+25A0), but you could use s itself or any symbol
        const char* symbol = (s == 0 ? "  " : "■ ");

        // choose color (fallback to reset if s out of range)
        const char* color = (s >= 0 && s <= M ? colors[s] : colors[0]);

        std::cout << color << symbol << "\033[0m";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
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

/////////////////////








void printVector(const std::vector<int>& vec) {
    for (const auto& v : vec) {
        std::cout << v << " ";
    }
    std::cout << std::endl;
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

double crystalParameter(std::vector<int> nodes, std::vector<std::vector<int>> adj, std::vector<int> sublattice_locations) {
    std::vector<int> occupancy_nodes;
    for (int i = 0; i < nodes.size(); i++) {
        if (nodes[i] == 0) {
            occupancy_nodes.push_back(0);
        }
        else {
            occupancy_nodes.push_back(1);
        }
    }

    double one = 0;
    double two = 0;
    double three = 0;

    double one_total = 0;
    double two_total = 0;
    double three_total = 0;

    for (int u = 0; u < adj.size(); u++) {
        if (sublattice_locations[u] == 1) {
            one_total++;
            bool tester = false;
            for (int v : adj[u]) {
                if (occupancy_nodes[u] == occupancy_nodes[v]) {
                    tester = true;
                }
            }
            if (tester == false) {
                one++;
            }
        }
        if (sublattice_locations[u] == 2) {
            two_total++;
            bool tester = false;
            for (int v : adj[u]) {
                if (occupancy_nodes[u] == occupancy_nodes[v]) {
                    tester = true;
                }
            }
            if (tester == false) {
                two++;
            }
        }
        if (sublattice_locations[u] == 3) {
            three_total++;
            bool tester = false;
            for (int v : adj[u]) {
                if (occupancy_nodes[u] == occupancy_nodes[v]) {
                    tester = true;
                }
            }
            if (tester == false) {
                three++;
            }
        }
    }

    double val_one = one / one_total;
    double val_two = two / two_total;

    if (three_total == 0) {
        return std::max(val_one, val_two);
    }
    double val_three = three / three_total;

    return std::max(val_one, std::max(val_two, val_three));
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

double demixedParameter2(std::vector<int> nodes, int M) {
    std::vector<int> number_species;
    for (int i = 1; i <= M; i++) {
        int count = 0;
        for (int n : nodes) {
            if (i == n) {
                count++;
            }
        }
        number_species.push_back(count);
    }
    double max = *std::max_element(number_species.begin(), number_species.end());
    return (max / nodes.size()) - (density(nodes) / M);
}

double mean(std::vector<double> arr) {
    double total = 0;
    for (double val : arr) {
        total += val;
    }
    return total / arr.size();
}

double variance(std::vector<double> arr) {
    double mean_val = mean(arr);
    double total = 0;
    for (double val : arr) {
        total += pow(val - mean_val, 2);
    }
    return total / arr.size();
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

// from before:
std::vector<std::vector<int>>
adjListToGrid(const std::vector<std::vector<int>>& adjList) {
    int N = adjList.size();
    int L = static_cast<int>(std::sqrt(N));
    if (L*L != N)
        throw std::invalid_argument("adjList size is not a perfect square");
    std::vector<std::vector<int>> grid(L, std::vector<int>(L));
    for (int idx = 0; idx < N; ++idx) {
        int r = idx / L;
        int c = idx % L;
        grid[r][c] = idx;
    }
    return grid;
}

// new helper: map your node‐values onto an L×L grid
template<typename T>
std::vector<std::vector<T>>
nodeValuesToGrid(const std::vector<T>& nodeVals,
                 const std::vector<std::vector<int>>& adjList)
{
    int N = adjList.size();
    int L = static_cast<int>(std::sqrt(N));
    if (L*L != N)
        throw std::invalid_argument("adjList size is not a perfect square");

    // get the 1D‐index → (r,c) map
    auto grid = adjListToGrid(adjList);

    // now build your L×L value‐grid
    std::vector<std::vector<T>> valueGrid(L, std::vector<T>(L));
    for (int r = 0; r < L; ++r) {
        for (int c = 0; c < L; ++c) {
            int idx = grid[r][c];        // original node index
            valueGrid[r][c] = nodeVals[idx];
        }
    }
    return valueGrid;
}




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
    MyArgs args = argparse::parse<MyArgs>(argc, argv);

    int L = args.L;
    int M = args.M;
    double z = args.z;
    string lat = args.lat;
    int SWEEPS = args.sweeps;
    
    int sweeps = SWEEPS; // will be modified during runtime when equilibrium point is reached
    int sample_size = 100000;

    int k = 0; // number of colors (or sublattices) in lattice graph, will be set to 2 or 3 depending on the k-partiteness of the lattice

    if (L <= 0 || M <= 0 || z <= 0) {
        std::cerr << "Error: All parameters must be positive values." << std::endl;
        return 1;
    }

    /*
    std::ofstream of_cp("output_cp.txt");
    if (!of_cp.is_open()) {
        std::cerr << "Failed to open output_cp.txt\n";
        return 1;
    }
    std::ofstream of_cp_2("output_cp_extra.txt");
    if (!of_cp.is_open()) {
        std::cerr << "Failed to open output_cp_extra.txt\n";
        return 1;
    }
    std::ofstream of_de("density.txt");
    if (!of_de.is_open()) {
        std::cerr << "Failed to open density.txt\n";
        return 1;
    }
    */

    std::ofstream node_coloring("node_color_data.txt");
    if (!node_coloring.is_open()) {
        std::cerr << "Failed to open node_color_data.txt\n";
        return 1;
    }

    std::vector<std::vector<int>> lattice_adjacency_list;

    std::string adj_data_file = "src/lattice/adj-lists/adj_list_" + std::to_string(L) + "_" + args.lat + ".txt";
    std::ifstream file(adj_data_file);

    std::cout << "Accessing lattice adjacency list" << std::endl;

    std::ifstream infile(adj_data_file.c_str());

    if (!infile) {
        std::cerr << "Error opening " << adj_data_file << std::endl;
        return 1;
    }

    std::cout << "Accessing done!" << std::endl;
    string line;

    while (std::getline(infile, line)) {
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

    std::vector<int> sublattice_locations = backtrackGraphColoring(lattice_adjacency_list, k, lattice_adjacency_list.size());
    
    if (isBipartite(lattice_adjacency_list)) {
        std::cout << "bipartite" << std::endl;
    } else {
        if (sublattice_locations.size() == 0) {
            std::cerr << "likely 4-colorable, NOT GOOD" << std::endl;
            exit(1);
        }
        else {
            std::cout << "tripartite" << std::endl;
        }
    }



    std::vector<int> nodes(lattice_adjacency_list.size(), 0);

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

    std::uniform_real_distribution<double> uni(0.0, 1.0);
    std::cout << "Running simulation with parameters: L = " << L << ", M = " << M << ", z = " << z << ", Lattice Type: " << lat << std::endl;

    double p_remove = 1.0/(M * std::min(1.0/z, 1.0));
    std::bernoulli_distribution bernoulli_trial_cluster(p_remove); // for cluster flipping

    NonBlockingTerminal nbt;
    bool keyPressed = false;

    std::vector<double> density_vals;

    while (s <= sweeps && !keyPressed) {

        if (nbt.kbhit()) {
            cout << "\nKey pressed. Breaking loop and finalizing..." << std::endl;
            nbt.getch(); // consume the character from the input buffer
            keyPressed = true;
            continue; // exit the current sweep and let the while condition terminate
        }

        for (int m = 0; m < nodes.size(); m++) {
            int i = randInt(0, nodes.size()-1);
            int k = randInt(0, M);

            
            int x = randInt(0, nodes.size()-1);
            
            bool isCluster = false;

            if (nodes[x] != 0) {    // if randomly selected site has something in it
                for (int j = 0; j < lattice_adjacency_list[x].size(); j++) {
                    int index = lattice_adjacency_list[x][j];
                    if (nodes[x] == nodes[index]) {
                        isCluster = true;
                        break;
                    }
                }
            }

            if (isCluster == true) { 
                bool prob_cluster_flipping = bernoulli_trial_cluster(rng); // decide whether to do cluster flipping or single site modification
                
                if (prob_cluster_flipping == true) {
                    std::vector<int> cluster = clusterFinder(nodes, lattice_adjacency_list, x);

                    int col = randIntWithoutVal(1, M, nodes[x]);
                    for (int v : cluster) { 
                        nodes[v] = col;
                    }
                    continue;
                }
                else {
                    nodes[x] = 0;
                    continue;
                }
            }
            
            
            // std::cout << colors[3] << "Single site modification at site: " << i << "\033[0m" << std::endl;

            bool conflict = false;
            for (int j = 0; j < lattice_adjacency_list[i].size(); j++) {
                int index = lattice_adjacency_list[i][j];
                if (k != nodes[index] && nodes[index] != 0) {
                    conflict = true;
                    break;
                }
            }

            if (nodes[i] == 0) { // if current nodes(i) site is empty
                double insertion_test = z;
                if (conflict == false && uni(rng) < insertion_test) { // rand(0,1) must be less than z
                    nodes[i] = k; // accept move
                }
                else {
                    continue; // if probability not reached then leave site unchanged
                }
            }
            bool allEmpty = true;
            for (int v : lattice_adjacency_list[i]) {
                if (nodes[v] != 0) {
                    allEmpty = false;
                    break;
                }
            }
            if (nodes[i] != 0) { // if current lattice(i,j) site has a particle
                if (k == 0) {
                    double removal_test = 1/z;
                    if (uni(rng) < removal_test) { // rand(0, 1) must be less than 1/z
                        nodes[i] = k; // accept move
                    }
                    else {
                        continue; // if probability not reached then leave site unchanged
                    }
                }
                else {
                    if (allEmpty) { // replace particle if there is no resulting conflict
                        nodes[i] = k;
                    }
                }
            }

        }
        
        // double param = crystalParameter(nodes, lattice_adjacency_list, sublattice_locations);
        double param2 = demixedParameter(nodes, M);
        double param = crystalParameter(nodes, lattice_adjacency_list, sublattice_locations);
        
        /*
        if (s >= 15000) {
            double param2 = density(nodes);
            density_vals.push_back(param2);
        }
        */

    
        // of_cp << param << std::endl;
        // of_cp_2 << param2 << std::endl;

        // of_de << density(nodes) << std::endl;
        std::cout << s << std::endl;     
        s++;
    }
    
    
    for (int k = 0; k < nodes.size(); k++) {
        node_coloring << nodes[k] << std::endl;
    }
    
    
    std::string disp_lat =
    "python src/lattice/lattice_display.py -L "
        + std::to_string(L)
        + " -l "
        + lat;

    FILE* oth = popen(disp_lat.c_str(), "r");
    if (!oth) {
        std::cerr << "Failed to execute command" << std::endl;
        return 1;
    }
    pclose(oth);
    

    // of_cp.close();
    // of_cp_2.close();
    // of_de.close();
    node_coloring.close();

    return 0;
}
