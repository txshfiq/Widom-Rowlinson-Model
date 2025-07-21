#include <openrand/philox.h>
#include <argparse/argparse.hpp>
#include <bits/stdc++.h>
#include <complex>
#include <algorithm>


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
    ./main --L 15 --M 15 --z 6 --lat square


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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
    MyArgs args = argparse::parse<MyArgs>(argc, argv);

    int L = args.L;
    int M = args.M;
    double z = args.z;
    string lat = args.lat;
    
    int sweeps = 10000; // will be modified during runtime when equilibrium point is reached
    int sample_size = 210000;

    int k = 0; // number of colors (or sublattices) in lattice graph, will be set to 2 or 3 depending on the k-partiteness of the lattice

    if (L <= 0 || M <= 0 || z <= 0) {
        std::cerr << "Error: All parameters must be positive values." << std::endl;
        return 1;
    }

    std::ostringstream oss;
    oss.str("");
    oss << std::fixed << std::setprecision(1) << z;
    std::string str_z = oss.str();
    std::string str_z_noformat = str_z; 
    
    for (char &c : str_z) {
        if (c == '.') {
            c = '-';
        }
    }

    std::string output_auto_cp = "sampling/sampling_L" + std::to_string(L) + "_M" + std::to_string(M) + "_z" + str_z + "_" + args.lat + ".txt";

    std::ofstream of_auto_cp(output_auto_cp.c_str());
    if (!of_auto_cp.is_open()) {
        std::cerr << "Failed to open sampling (used for autocorrelation) text file\n";
        return 1;
    }
    
    std::vector<std::vector<int>> lattice_adjacency_list;

    std::string adj_data_file = "adj-lists/adj_list_" + std::to_string(L) + "_" + args.lat + ".txt";
    std::ifstream file(adj_data_file);

    if (!file) {
        std::string gen_lat =
            "python lattice_generation.py -L "
            + std::to_string(L)
            + " -l "
            + lat;     

        FILE* generator = popen(gen_lat.c_str(), "r");
        if (!generator) {
            std::cerr << "Failed to execute lattice_generation.py" << std::endl;
            return 1;
        }
        pclose(generator);
    }

    std::ifstream infile(adj_data_file.c_str());

    if (!infile) {
        std::cerr << "Error opening " << adj_data_file << std::endl;
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
    // std::vector<int> sublattice_locations = backtrackGraphColoring(lattice_adjacency_list, k, lattice_adjacency_list.size());

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

    double p_remove = 1.0/(M * std::min(1.0/z, 1.0));
    std::bernoulli_distribution bernoulli_trial_cluster(p_remove); // for cluster flipping


    // std::cout << "Running simulation with parameters: L = " << L << ", M = " << M << ", z = " << z << std::endl;

    while (s <= sweeps) {
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
        double param = demixedParameter(nodes, M);
        
        if (equilibrium_point_found == false) {
            if (s % block_size != 0) {
                crystal_parameters_test.push_back(param);
            }
            else {
                if (variance(crystal_parameters_test) < critical_variance) {
                    // std::cout << colors[2] << "Critical point reached at sweep " << s << " with variance: " << variance(crystal_parameters_test) << " sample size: " << sample_size << "\033[0m" << std::endl;
                    equilibrium_point_found = true; // set equilibrium point found to true
                    sweeps = s + sample_size;
                }
                else if (s >= sweeps) {
                    // std::cout << colors[1] << "Maximum sweeps reached without finding equilibrium point, will start collecting data..." << " sample size: " << sample_size << "\033[0m" << std::endl;
                    equilibrium_point_found = true; // set equilibrium point found to true
                    sweeps = s + sample_size;
                }
                else {
                    // std::cout << colors[1] << "Variance too high at sweep " << s << ", continuing simulation..." << "\033[0m" << std::endl;
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

    // std::cout << colors[2] << "Data collection complete. Total sweeps: " << s-1 << "\033[0m" << std::endl;

    
    // std::cout << colors[4] << "Calculating autocorrelation time using Python script..." << "\033[0m" << std::endl;

    std::string command =
            "python script_auto.py -L "
            + std::to_string(L)
            + " -l "
            + lat;
            + " -M "
            + std::to_string(M);
            + " -z "
            + str_z_noformat;             
    FILE* in = popen(command.c_str(), "r");
    if (!in) {
        std::cerr << "Failed to execute script_auto.py" << std::endl;
        return 1;
    }

    double value;
    while (fscanf(in, "%lf", &value) == 1) {
        // value is being read
    }

    // std::cout << colors[5] << "Autocorrelation time (in sweeps): " << value << "\033[0m" << std::endl;

    
    
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
    remove(output_auto_cp.c_str());    
    

    return 0;
}
