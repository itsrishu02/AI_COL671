#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

void addClause(vector<vector<int>>& clauses, const vector<int>& clause) {
    clauses.push_back(clause);
}

void addCliquePositionClauses(vector<vector<int>>& clauses, int n, int k1, int k2) {
    for (int i = 1; i <= k1 + k2; ++i) {
        vector<int> clause;
        for (int u = 1; u <= n; ++u) {
            clause.push_back(u + (i - 1) * n);
        }
        addClause(clauses, clause);
    }
    
}

void addNoSamePositionClauses(vector<vector<int>>& clauses, int n, int k1, int k2) {
    for (int i = 1; i <= k1 + k2; ++i) {
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

void addEdgeConstraints(vector<vector<int>>& clauses, int n, int k1, int k2, const vector<pair<int, int>>& edges) {
    for (int i = 1; i <= k1 + k2; ++i) {
        for (int j = i + 1; j <= k1 + k2; ++j) {
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
void addEdgeBelongConstraints(vector<vector<int>>& clauses, int n, int k1, int k2, const vector<pair<int, int>>& edges) {
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
                            addClause(clauses, clause);
                        }
                    }
                }
            }
            if (!edgeExists) {
                for (int i = 1; i <= k2; ++i) {
                    for (int j = 1; j <= k2; ++j) {
                        if (i != j) {
                            vector<int> clause;
                            clause.push_back(-((u + (i - 1) * n)+(n*k1)));
                            clause.push_back(-((v + (j - 1) * n)+(n*k1)));
                            addClause(clauses, clause);
                        
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
    
    ofstream output(te+".satinput"); 

    int n, m, k1, k2;
    input >> n >> m >> k1 >> k2;

    vector<pair<int, int>> edges;
    for (int i = 0; i < m; ++i) {
        int u, v;
        input >> u >> v;
        edges.push_back({u, v});
    }

    vector<vector<int>> cnfClauses;

    addCliquePositionClauses(cnfClauses, n, k1, k2);
    addNoSamePositionClauses(cnfClauses, n, k1, k2);
    addEdgeConstraints(cnfClauses, n, k1, k2, edges);
    addEdgeBelongConstraints(cnfClauses, n, k1, k2, edges);

    // Output the CNF-SAT formula to the output file
    output << "p cnf " << n * (k1 + k2) << " " << cnfClauses.size() << endl;
    for (const auto& clause : cnfClauses) {
        for (int literal : clause) {
            output << literal << " ";
        }
        output << "0" << endl;
    }

    input.close();
    output.close();

    return 0;
}
