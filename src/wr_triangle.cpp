#include <iostream>
#include <random>

// M = # of species
// L = lattice size (L x L) of triangular rhombus-shaped lattice
// z = fugacity (absolute activity) -> constant value, same chemical potential throughout (grand-canonical ensemble)

const int L = 20;
const int M = 29;
const double z = 5;

int lattice[L][L];

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



void printLattice(int arr[][L]) {
    const int CELL_W = 2;      // every cell prints as 2 chars: "■ " or "  "
    for (int i = 0; i < L; ++i) {
        // for a vertical-mirror slant, we indent *decreasing* each row
        // (if you wanted the other slant, just use `i*(CELL_W/2)` here instead)
          int pad = i * (CELL_W/2);
        std::cout << std::string(pad, ' ');
        
        for (int j = 0; j < L; ++j) {
            int s = arr[i][j];
            const char* symbol = (s == 0 ? "  " : "■ ");
            const char* color  = (s >= 0 && s <= M ? colors[s] : colors[0]);
            std::cout << color << symbol << "\033[0m";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}




double crystalParameter(int arr[][L], int i, int j) {
    double total = 0;
    for (int i = 1; i <= L; i++) {
        for (int j = 1; j <= L; j++) {
            int M = 0;
            if (arr[i-1][j-1] != 0) {
                M = 1;
            }
            total += ((2*M - 1)*(pow(-1, (i+j))));
        } 
    }
    return (total/(L*L));
}

int randInt(int x, int y) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(x, y);
    return dist(gen);
}


int main() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution bernoulli_trial((M*z)/((M*z)+1));
    for (int i = 0; i < L; i++) {
        for (int j = 0; j < L; j++) {
            bool success = bernoulli_trial(gen);
            if (success == false) { // if bernoulli probability outcomes false, make lattice(i,j) empty (0)
                lattice[i][j] = 0;
                continue; // move to next iteration
            }
            else {
                int k = randInt(1, M); // generate species (k = 1, 2, 3, ... , M)
                bool conflict = false;
                if (i-1 >= 0) { // looks at left adjacent box
                    if (lattice[i-1][j] != k && lattice[i-1][j] != 0) { // if lattice(i-1,j) ≠ k and empty, there is a conflict
                        conflict = true;
                    }
                }
                if (j-1 >= 0) { // looks at up adjacent box
                    if (lattice[i][j-1] != k && lattice[i][j-1] != 0) { // if lattice(i,j-1) ≠ k and empty, there is a conflict
                        conflict = true;
                    }
                }
                if (i-1 >= 0 && j+1 < L) { // looks at up adjacent box
                    if (lattice[i-1][j+1] != k && lattice[i-1][j+1] != 0) { // if lattice(i,j-1) ≠ k and empty, there is a conflict
                        conflict = true;
                    }
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
    
    int sweeps = 0;

    std::uniform_real_distribution<double> uni(0.0, 1.0);

    while (sweeps < 10000) {
        for (int m = 0; m < (L*L); m++) {
            int i = randInt(0, L-1);
            int j = randInt(0, L-1);
            int k = randInt(0, M);
            
            bool conflict = false;
            if (i-1 >= 0) { // looks at left adjacent box
                if (lattice[i-1][j] != k && lattice[i-1][j] != 0) { // if lattice(i-1,j) ≠ k and empty, there is a conflict
                    conflict = true;
                }
            }
            if (j-1 >= 0) { // looks at up adjacent box
                if (lattice[i][j-1] != k && lattice[i][j-1] != 0) { // if lattice(i,j-1) ≠ k and empty, there is a conflict
                    conflict = true;
                }
            }
            if (i+1 < L) { // looks at right adjacent box
                if (lattice[i+1][j] != k && lattice[i+1][j] != 0) { // if lattice(i+1,j) ≠ k and empty, there is a conflict
                    conflict = true;
                }
            }
            if (j+1 < L) { // looks at down adjacent box
                if (lattice[i][j+1] != k && lattice[i][j+1] != 0) { // if lattice(i,j+1) ≠ k and empty, there is a conflict
                    conflict = true;
                }
            }
            if (i-1 >= 0 && j+1 < L) { 
                if (lattice[i-1][j+1] != k && lattice[i-1][j+1] != 0) { 
                    conflict = true;
                }
            }
            if (i+1 < L && j-1 >= 0) { 
                if (lattice[i+1][j-1] != k && lattice[i+1][j-1] != 0) { 
                    conflict = true;
                }
            }
            if (lattice[i][j] == 0) { // if current lattice(i,j) site is empty
                double insertion_test = z;
                if (conflict == false && uni(gen) < insertion_test) { // rand(0,1) must be less than z
                    lattice[i][j] = k; // accept move
                }
                else {
                    continue; // if probability not reached then leave site unchanged
                }
            }
            if (lattice[i][j] != 0) { // if current lattice(i,j) site has a particle
                if (k == 0) {
                    double removal_test = 1/z;
                    if (uni(gen) < removal_test) { // rand(0, 1) must be less than 1/z
                        lattice[i][j] = k; // accept move
                    }
                    else {
                        continue; // if probability not reached then leave site unchanged
                    }
                }
                else {
                    if (conflict == false) { // replace particle if there is no resulting conflict
                        lattice[i][j] = k;
                    }
                    
                }
            }
        }
        printLattice(lattice);
        sweeps += 1;
    }
    std::cout << crystalParameter(lattice, L, L) << std::endl;
    

    return 0;
}
