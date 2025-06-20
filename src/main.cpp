#include <iostream>
#include <fstream>
#include <random>
#include <openrand/philox.h>
#include <argparse/argparse.hpp>
#include <cstdio>
#include <cmath>
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


    g++ -std=c++17 -I./include src/main.cpp -o main -lstdc++fs -g
    ./main --L 40 --M 9 --z 0.5


*/

const int r = 30;

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


void printVector(const std::vector<int>& vec) {
    for (int val : vec) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
}

std::vector<std::vector<int>> renderSquare(const std::vector<int>& nodes, const std::vector<std::vector<int>>& adj_list) {
    std::vector<std::vector<int>> arr;
    return arr;
}

template<typename T>
void print2D(const std::vector<std::vector<T>>& mat) {
    std::cout << "[";
    for (size_t i = 0; i < mat.size(); ++i) {
        std::cout << "[";
        for (size_t j = 0; j < mat[i].size(); ++j) {
            std::cout << mat[i][j];
            if (j + 1 < mat[i].size()) 
                std::cout << ", ";
        }
        std::cout << "]";
        if (i + 1 < mat.size()) 
            std::cout << ", ";
    }
    std::cout << "]\n";
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

int roundDownToNearestTen(double value) {
    return std::floor(value / 10.0)*10.0;
}

/*
double square_crystalParameter(std::vector<int> nodes, std::vector<std::vector<int>> adj_list) {
    int L = nodes.size();
    double total = 0;
    for (int i = 1; i <= L; i++) {
        for (int j = 1; j <= L; j++) {
            int M = 0;
            if (arr[i-1][j-1] != 0) {
                M = 1;
            }
            total += (((2*M) - 1)*pow(-1, i+j)); 
        } 
    }
    return (total/(L*L));
}
*/

/*
double density(std::vector<std::vector<int>> arr) {
    int L = arr.size();
    double total = 0;
    for (int i = 0; i < L; i++) {
        for (int j = 0; j < L; j++) {
            if (arr[i][j] != 0) {
                total += 1;
            }
        }
    }
    return total / (L * L);
}
*/

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
    std::random_device rd;
    uint64_t seed = (static_cast<uint64_t>(rd()) << 32)
                  | static_cast<uint64_t>(rd());
    openrand::Philox rng(seed, 0);
    
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
    
    int sweeps = 100000; // will be modified during runtime when equilibrium point is reached
    int sample_size = 5000000;

   
    if (L <= 0 || M <= 0 || z <= 0) {
        std::cerr << "Error: All parameters must be positive values." << std::endl;
        return 1;
    }

    std::ofstream of_cp("output_cp.txt");
    if (!of_cp.is_open()) {
        std::cerr << "Failed to open output_cp.txt\n";
        return 1;
    }

    std::ofstream of_de("output_de.txt");
    if (!of_de.is_open()) {
        std::cerr << "Failed to open output_de.txt\n";
        return 1;
    }

    std::ofstream node_coloring("node_color_data.txt");
    if (!of_cp.is_open()) {
        std::cerr << "Failed to open node_color_data.txt\n";
        return 1;
    }

    /*
    std::ofstream of_auto_cp("output_auto_cp.txt");
    if (!of_auto_cp.is_open()) {
        std::cerr << "Failed to open output_auto_cp.txt\n";
        return 1;
    }

    std::ofstream of_auto_de("output_auto_de.txt");
    if (!of_auto_de.is_open()) {
        std::cerr << "Failed to open output_auto_de.txt\n";
        return 1;
    }
    */

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



    /*
    while (std::getline(infile, line)) {
        std::vector<int> adj_list_push_back;
        adj_list_push_back.push_back(cn);
        std::string temp_line = "";
        for (char c : line) {
            if (c != '[' && c != ']' && c != ',') {
                temp_line += c;
            }
        }
        
        std::string temp_num = "";

        for (char c : temp_line) {
            if (c != ' ') {
                temp_num += c;
            }
            else {
                int n = std::stoi(temp_num);
                adj_list_push_back.push_back(n);
                temp_num.erase();
            }
        }
        lattice_adjacency_list.push_back(adj_list_push_back);
        cn++;
    }
    */

    std::vector<int> nodes(lattice_adjacency_list.size(), 0);

    remove("temp_lattice_data.txt");

    
    std::random_device rd;
    uint64_t seed = (static_cast<uint64_t>(rd()) << 32)
                  | static_cast<uint64_t>(rd());
    openrand::Philox rng(seed, 0);

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
    
    /*
    for (int i = 0; i < L; i++) {
        for (int j = 0; j < L; j++) {
            bool success = bernoulli_trial(rng);
            if (success == false) { // if bernoulli probability outcomes false, make lattice(i,j) empty (0)
                lattice[i][j] = 0;
                continue; // move to next iteration
            }
            else {
                int k = randInt(1, M); // generate species (k = 1, 2, 3, ... , M)
                bool conflict = false;
                if (lattice[mod(i-1, L)][j] != k && lattice[mod(i-1, L)][j] != 0) { // if lattice(i-1,j) ≠ k and empty, there is a conflict
                    conflict = true;
                }
                if (lattice[i][mod(j-1, L)] != k && lattice[i][mod(j-1, L)] != 0) { // if lattice(i,j-1) ≠ k and empty, there is a conflict
                    conflict = true;
                }
                
                if (conflict == false) { // if there is no conflict, use k as lattice(i,j)
                    lattice[i][j] = k;
                }
                else {                   // if there IS a conflict, make lattice(i,j) empty (0)
                    lattice[i][j] = 0;
                }
            }
        }
    }
    */
    
    int s = 1; // start at sweep 1

    std::uniform_real_distribution<double> uni(0.0, 1.0);

    vector<double> crystal_parameters_test;
    vector<double> crystal_parameters_eq;
    vector<double> density_eq;

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
                    if (conflict == false) { // replace particle if there is no resulting conflict
                        nodes[i] = k;
                    }
                }
            }
        }
        /*
        printVector(nodes); 
        std::cout << s << std::endl; 
        */

        // double p = crystalParameter(lattice);
        // double d = density(lattice);

        // of_cp << p << std::endl;
        // of_de << d << std::endl;

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
        */
        /*
        if (s >= critical_sweeps) {
            if (s < sweeps) {
                crystal_parameters_eq.push_back(p);
                density_eq.push_back(d);

                //
                of_auto_cp << p << std::endl;
                of_auto_de << d << std::endl;
                //
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

    // std::cout << colors[2] << "Data collection complete. Total sweeps: " << s-1 << "\033[0m" << std::endl;

    /*
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
    */
    
    /*
    int decorrelationTime = 10;   // int decorrelationTime = roundDownToNearestTen(2*value); ---> replaced with sampling every 10th sweep
    int counterForAverage = 0;

    double fourthOrderAvg = 0;
    double secondOrderAvg = 0;
    for (int i = 1; i <= crystal_parameters_eq.size(); i++) {
        if (i % decorrelationTime == 0) {
            counterForAverage++;
            fourthOrderAvg += pow(crystal_parameters_eq[i-1],4);
            secondOrderAvg += pow(crystal_parameters_eq[i-1],2);
        }
    }

    fourthOrderAvg /= counterForAverage;
    secondOrderAvg /= counterForAverage;
    
    double cumulant = 1 - (fourthOrderAvg/(3*pow(secondOrderAvg, 2)));

    // std::cout << colors[3] << "Binder cumulant: " << cumulant << "\033[0m" << std::endl;
    std::cout << cumulant << std::endl;

    */

    of_cp.close();
    of_de.close();

    /*
    of_auto_cp.close();
    of_auto_de.close();
    */

    
    remove("output_cp.txt");
    remove("output_de.txt");

    /*
    remove("output_auto_cp.txt");
    remove("output_auto_de.txt");
    */
    

    return 0;
}
