#include <openrand/philox.h>
#include <argparse/argparse.hpp>
#include <bits/stdc++.h>

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


    g++ -std=c++17 -I./include src/main.cpp -o main -lstdc++fs -g
    ./main --L 15 --M 15 --z 6 --lat square


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

    std::vector<bool> visited(adj.size(), false);
    std::queue<int> q;

    visited[0] = true;
    q.push(0);

    double one = 0;
    double two = 0;
    double three = 0;

    double one_total = 0;
    double two_total = 0;
    double three_total = 0;

    while (!q.empty()) {
        int u = q.front(); q.pop();

        if (sublattice_locations[u] == 1) {
            one_total++;
            bool tester = false;
            for (int v : adj[u]) {
                if (!visited[v]) {
                    visited[v] = true;
                    q.push(v);
                }
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
                if (!visited[v]) {
                    visited[v] = true;
                    q.push(v);
                }
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
                if (!visited[v]) {
                    visited[v] = true;
                    q.push(v);
                }
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
    MyArgs args = argparse::parse<MyArgs>(argc, argv);

    int L = args.L;
    int M = args.M;
    double z = args.z;
    string lat = args.lat;
    
    int sweeps = 10000; // will be modified during runtime when equilibrium point is reached
    int sample_size = 100000;

    int k = 0; // number of colors (or sublattices) in lattice graph, will be set to 2 or 3 depending on the k-partiteness of the lattice

    if (L <= 0 || M <= 0 || z <= 0) {
        std::cerr << "Error: All parameters must be positive values." << std::endl;
        return 1;
    }

    std::ofstream of_cp("output_cp.txt");
    if (!of_cp.is_open()) {
        std::cerr << "Failed to open output_cp.txt\n";
        return 1;
    }

    std::ofstream of_auto_cp("output_auto_cp.txt");
    if (!of_auto_cp.is_open()) {
        std::cerr << "Failed to open output_auto_cp.txt\n";
        return 1;
    }
    
    std::vector<std::vector<int>> lattice_adjacency_list;

    std::string gen_lat =
    "python lattice_generation.py -L "
        + std::to_string(L)
        + " -l "
        + lat;        

    FILE* generator = popen(gen_lat.c_str(), "r");
    if (!generator) {
        std::cerr << "Failed to execute command" << std::endl;
        return 1;
    }
    pclose(generator);

    std::ifstream infile("temp_lattice_data.txt");

    if (!infile) {
        std::cerr << "Error opening file!" << std::endl;
        return 1;
    }

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

    std::vector<int> nodes(lattice_adjacency_list.size(), 0);
    std::vector<int> sublattice_locations = backtrackGraphColoring(lattice_adjacency_list, k, lattice_adjacency_list.size());

    remove("temp_lattice_data.txt");

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

    vector<double> crystal_parameters_test;
    vector<double> crystal_parameters_eq;
    vector<double> crystal_parameters_sampling;

    double critical_variance = 0.0005; // critical variance for equilibrium point detection
    int block_size = 2500; // block size for variance calculation

    bool equilibrium_point_found = false; // flag to indicate if equilibrium point has been found

    // std::cout << "Running simulation with parameters: L = " << L << ", M = " << M << ", z = " << z << std::endl;

    while (s <= sweeps) {
        for (int m = 0; m < nodes.size(); m++) {
            int i = randInt(0, nodes.size()-1);
            int k = randInt(0, M);
            
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
        
        double param = crystalParameter(nodes, lattice_adjacency_list, sublattice_locations);
        of_cp << param << std::endl;
        
        if (equilibrium_point_found == false) {
            if (s % block_size != 0) {
                crystal_parameters_test.push_back(param);
            }
            else {
                if (variance(crystal_parameters_test) < critical_variance) {
                    std::cout << colors[2] << "Critical point reached at sweep " << s << " with variance: " << variance(crystal_parameters_test) << " sample size: " << sample_size << "\033[0m" << std::endl;
                    equilibrium_point_found = true; // set equilibrium point found to true
                    sweeps = s + sample_size;
                }
                else if (s >= sweeps) {
                    std::cout << colors[1] << "Maximum sweeps reached without finding equilibrium point, will start collecting data..." << " sample size: " << sample_size << "\033[0m" << std::endl;
                    equilibrium_point_found = true; // set equilibrium point found to true
                    sweeps = s + sample_size;
                }
                else {
                    std::cout << colors[1] << "Variance too high at sweep " << s << ", continuing simulation..." << "\033[0m" << std::endl;
                }
                crystal_parameters_test.clear(); // clear the test array for next variance calculation
            }
        }
    
        if (s < sweeps && equilibrium_point_found == true) {
            crystal_parameters_sampling.push_back(param);
            of_auto_cp << param << std::endl;
        }
        
        s++;
    }

    std::cout << colors[2] << "Data collection complete. Total sweeps: " << s-1 << "\033[0m" << std::endl;

    
    std::cout << colors[4] << "Calculating autocorrelation time using Python script..." << "\033[0m" << std::endl;

    std::string command = "python script_auto.py"; 
    FILE* in = popen(command.c_str(), "r");
    if (!in) {
        std::cerr << "Failed to execute command" << std::endl;
        return 1;
    }

    double value;
    while (fscanf(in, "%lf", &value) == 1) {
        std::cout << value << std::endl;
    }

    std::cout << colors[5] << "Autocorrelation time (in sweeps): " << value << "\033[0m" << std::endl;

    
    
    int decorrelationTime = 10;   // int decorrelationTime = roundDownToNearestTen(2*value); ---> replaced with sampling every 10th sweep
    int counterForAverage = 0;

    double fourthOrderAvg = 0;
    double secondOrderAvg = 0;
    for (int i = 1; i <= crystal_parameters_sampling.size(); i++) {
        if (i % decorrelationTime == 0) {
            counterForAverage++;
            fourthOrderAvg += pow(crystal_parameters_sampling[i-1],4);
            secondOrderAvg += pow(crystal_parameters_sampling[i-1],2);
        }
    }

    fourthOrderAvg /= counterForAverage;
    secondOrderAvg /= counterForAverage;
    
    double cumulant = 1 - (fourthOrderAvg/(3*pow(secondOrderAvg, 2)));

    // std::cout << colors[3] << "Binder cumulant: " << cumulant << "\033[0m" << std::endl;
    std::cout << cumulant << std::endl;


    
    of_auto_cp.close();
    // remove("output_auto_cp.txt");
    
    

    return 0;
}
