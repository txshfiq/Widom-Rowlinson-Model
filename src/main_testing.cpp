#include <iostream>
#include <fstream>
#include <random>
#include <openrand/philox.h>
#include <argparse/argparse.hpp>
#include <cstdio>
#include <cmath>
#include <algorithm>
#include <queue>
#include <vector>
#include <string>
#include <sstream>

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


    g++ -std=c++17 -I./include src/main_testing.cpp -o main -lstdc++fs -g
    ./main --L 15 --M 15 --z 6 --lat square


*/

// seeding random number generator (Philox)
std::random_device rd;
uint64_t seed = (static_cast<uint64_t>(rd()) << 32) | static_cast<uint64_t>(rd());
openrand::Philox rng(seed, 0);

int roundDownToNearestTen(double value) {
    return std::floor(value / 10.0)*10.0;
}

double crystalParameter(std::vector<int> nodes, std::vector<std::vector<int>> adj) {
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
    int count = 0;
    int count_adj_diff = 0;

    while (!q.empty()) {
        int u = q.front(); q.pop();

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
            count_adj_diff++;
        }
    }
    return (double(count_adj_diff) / nodes.size());
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

int mod(int a, int b) {
    return ((a % b) + b) % b;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
    MyArgs args = argparse::parse<MyArgs>(argc, argv);

    int L = args.L;
    int M = args.M;
    double z = args.z;
    string lat = args.lat;
    
    int sweeps = 50000; // will be modified during runtime when equilibrium point is reached
    int sample_size = 100000;

   
    if (L <= 0 || M <= 0 || z <= 0) {
        std::cerr << "Error: All parameters must be positive values." << std::endl;
        return 1;
    }

    std::ofstream of_cp("output_cp.txt");
    if (!of_cp.is_open()) {
        std::cerr << "Failed to open output_cp.txt\n";
        return 1;
    }



    std::ofstream node_coloring("node_color_data.txt");
    if (!node_coloring.is_open()) {
        std::cerr << "Failed to open node_color_data.txt\n";
        return 1;
    }
    

    std::vector<std::vector<int>> lattice_adjacency_list;

    std::string gen_lat =
    "python lattice_generation.py -L "
        + std::to_string(L)
        + " -l "
        + lat;        

    FILE* in = popen(gen_lat.c_str(), "r");
    if (!in) {
        std::cerr << "Failed to execute command" << std::endl;
        return 1;
    }
    pclose(in);

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

    std::vector<int> nodes(lattice_adjacency_list.size(), 0);

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

    double critical_variance = 0.0005; // critical variance for equilibrium point detection
    int block_size = 2500; // block size for variance calculation
    int critical_sweeps = 1000000; // number of sweeps at which the critical point is reached, will be modified during runtime

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
    
        double param = crystalParameter(nodes, lattice_adjacency_list);

        of_cp << param << std::endl;

        /*
        if (equilibrium_point_found == false) {
            if (s % block_size != 0) {
                crystal_parameters_test.push_back(p);
            }
            else {
                if (variance(crystal_parameters_test) < critical_variance) {
                    // std::cout << colors[2] << "Critical point reached at sweep " << s << " with variance: " << variance(crystal_parameters_test) << " sample size: " << sample_size << "\033[0m" << std::endl;
                    critical_sweeps = s; // set critical sweeps to current sweep
                    equilibrium_point_found = true; // set equilibrium point found to true
                    sweeps = s + sample_size;
                }
                if (s >= sweeps) {
                    // std::cout << colors[1] << "Maximum sweeps reached without finding equilibrium point, will start collecting data..." << " sample size: " << sample_size << "\033[0m" << std::endl;
                    critical_sweeps = s; // set critical sweeps to current sweep
                    equilibrium_point_found = true; // set equilibrium point found to true
                    sweeps = s + sample_size;
                }
                else {
                    // std::cout << colors[1] << "Variance too high at sweep " << s << ", continuing simulation..." << "\033[0m" << std::endl;
                }
                crystal_parameters_test.clear(); // clear the test array for next variance calculation
            }
        }
        
        
        if (s >= critical_sweeps) {
            if (s < sweeps) {
                crystal_parameters_eq.push_back(p);
            }
        }
        */ 
        
        s++;
    }
    

    
    for (int k = 0; k < nodes.size(); k++) {
        node_coloring << nodes[k] << std::endl;
    }

    node_coloring.close();
    


    std::string disp_lat ="python lattice_display.py";    

    FILE* oth = popen(disp_lat.c_str(), "r");
    if (!oth) {
        std::cerr << "Failed to execute command" << std::endl;
        return 1;
    }
    pclose(oth);

    of_cp.close();
    

    return 0;
}
