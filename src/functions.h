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

/*
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
*/

std::vector<int> clusterFinder(std::vector<int> nodes, std::vector<std::vector<int>> adj, int start) {
    std::vector<bool> visited(nodes.size(), false);
    std::queue<int> q;

    std::vector<int> cluster_vertices;

    visited[start] = true;
    q.push(start);
    cluster_vertices.push_back(start);

    while (!q.empty()) {
        int u = q.front(); q.pop();

        bool tester = false;
        for (int v : adj[u]) {
            if (!visited[v] && nodes[u] == nodes[v]) {
                cluster_vertices.push_back(v);
                visited[v] = true;
                q.push(v);
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

int findRoot(int i, std::vector<int> &ptr) {
    int r = i;
    int s = i;
    while (ptr[r] >= 0) {
        ptr[s] = ptr[r];
        s = r;
        r = ptr[r];
    }
    return r;
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