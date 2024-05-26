#include <iostream>
#include<bits/stdc++.h>
#include <fstream>
#include <vector>
#include <sstream>

using namespace std;


int process_miniSAT_result_to_produce_output(const std::string& input_file, const std::string& output_file, int n, int k1) {
    std::ifstream infile(input_file);
    int k2 =0;

    std::string first_line;
    std::getline(infile, first_line);
    if (first_line == "UNSAT") {
        return (k1-1);
    }

    // For 'SAT' cases, read & store positive variables (sort first k1 and then k2 separately)
    else if (first_line == "SAT") {
        std::ofstream outfile(output_file);
        std::string second_line;
        std::getline(infile, second_line);
        std::istringstream iss(second_line);
        //std::cout << "Input File Line 2: " << second_line << std::endl;
        std::vector<int> X;
        int x;
        while (iss >> x) {
            if (x > 0) {
                if (x % n == 0) {
                    X.push_back(n);
                } else {
                    X.push_back(x % n);
                }
            }
        }

        std::vector<int> X_k1(X.begin(), X.begin() + k1);
        std::vector<int> X_k2(X.begin() + k1, X.end());
        std::sort(X_k1.begin(), X_k1.end());
        std::sort(X_k2.begin(), X_k2.end());
        X_k1.insert(X_k1.end(), X_k2.begin(), X_k2.begin() + k2);

        // Write output in desired format
        outfile << "#1" << std::endl;
        for (int i = 0; i < k1; ++i) {
            if(i==0)
            outfile << X_k1[i];
            else outfile << " "<<X_k1[i];
        }
        outfile << std::endl;
        return 0;
    }
}

void addClause(vector<vector<int>>& clauses, const vector<int>& clause) {
    clauses.push_back(clause);
}

void addCliquePositionClauses(vector<vector<int>>& clauses, int n, int k1) {
    for (int i = 1; i <= k1; ++i) {
        vector<int> clause;
        for (int u = 1; u <= n; ++u) {
            clause.push_back(u + (i - 1) * n);
        }
        addClause(clauses, clause);
    }
}

void addNoSamePositionClauses(vector<vector<int>>& clauses, int n, int k1) {
    for (int i = 1; i <= k1; ++i) {
        for (int u = 1; u <= n; ++u) {
            for (int v = u + 1; v <= n; ++v) {
                vector<int> clause;
                clause.push_back(-(u + (i - 1) * n));
                clause.push_back(-(v + (i - 1) * n));
                addClause(clauses, clause);
            }
        }
    }
}

void addEdgeConstraints(vector<vector<int>>& clauses, int n, int k1, const vector<pair<int, int>>& edges) {
    for (int i = 1; i <= k1; ++i) {
        for (int j = i + 1; j <= k1; ++j) {
            bool hasEdge = false;
            for (const auto& edge : edges) {
                int u = edge.first;
                int v = edge.second;
                int uPosition = (u - 1) / n + 1;
                int vPosition = (v - 1) / n + 1; 
                if ((uPosition == i && vPosition == j) || (uPosition == j && vPosition == i)) {
                    hasEdge = true;
                    break;
                }
            }
            if (!hasEdge) {
                // Exclude edges between positions i and j
                for (int u = 1; u <= n; ++u) {
                    vector<int> clause;
                    clause.push_back(-(u + (i - 1) * n));
                    clause.push_back(-(u + (j - 1) * n));
                    addClause(clauses, clause);
                }
            }
        }
    }
}
void addEdgeBelongConstraints(vector<vector<int>>& clauses, int n,int k1, const vector<pair<int, int>>& edges) {
    for (int u = 1; u <= n; ++u) {
        for (int v = u + 1; v <= n; ++v) {
            bool edgeExists = false;
            for (const auto& edge : edges) {
                if ((edge.first == u && edge.second == v) || (edge.first == v && edge.second == u)) {
                    edgeExists = true;
                    break;
                }
            }
            if (!edgeExists) {
                for (int i = 1; i <= k1; ++i) {
                    for (int j = 1; j <= k1; ++j) {
                        if (i != j) {
                            vector<int> clause;
                            clause.push_back(-(u + (i - 1) * n));
                            clause.push_back(-(v + (j - 1) * n));
                            clauses.push_back(clause);
                        }
                    }
                }
            }
        }
    }
}

int main(int argc, char** argv) {

    string te = argv[1];
    ifstream input( te+".graph" );

    int n, m, k1;
    input >> n >> m;

    vector<pair<int, int>> edges;
    for (int i = 0; i < m; ++i) {
        int u, v;
        input >> u >> v;
        edges.push_back({u, v});
    }

    int p;

    for(int i=1;i<=n;i++){
        k1=i;
        ofstream output("test.satcnf");
        vector<vector<int>> cnfClauses;
        addCliquePositionClauses(cnfClauses, n, k1);
        addNoSamePositionClauses(cnfClauses, n, k1);
        addEdgeConstraints(cnfClauses, n, k1, edges);
        addEdgeBelongConstraints(cnfClauses, n, k1, edges);
    // Output the CNF-SAT formula to the output file
    output << "p cnf " << n * k1 << " " << cnfClauses.size() << endl;
    for (const auto& clause : cnfClauses) {
        for (int literal : clause) {
            output << literal << " ";
        }
        output << "0" << endl;
    }
        output.close();

        string command = "./minisat test.satcnf test.satoutput";
        std::system(command.c_str());

        std::string input_file2 = "test.satoutput";
        std::string output_file = te+".mapping";
    
        p = process_miniSAT_result_to_produce_output(input_file2, output_file, n, k1);
        if(p != 0){
            output.close();
            break;
        }
    }
    

    input.close();
    

    return 0;
}
