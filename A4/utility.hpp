#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <vector>
#include <unordered_map> 
#include <bits/stdc++.h>

using namespace std;

extern unordered_map<string, int> string_to_idx;
extern unordered_map<int, string> idx_to_string;

class Graph_Node {

private:
    string Node_Name;
    vector<int> Children;
    vector<string> Parents;
    int nvalues;
    vector<string> values;
    vector<float> CPT;

public:
    // Declare member functions of the Graph_Node class
    Graph_Node(string name, int n, vector<string> vals);
    string get_name();
    vector<int> get_children();
    vector<string> get_Parents();
    vector<float> get_CPT();
    int get_nvalues();
    vector<string> get_values();
    void set_CPT(vector<float> new_CPT);
    void set_Parents(vector<string> Parent_Nodes);
    int add_child(int new_child_index);
    void random();
};

class network {

    vector<Graph_Node> Pres_Graph;
    
public:
    // Declare member functions of the network class
    int addNode(Graph_Node node);
    int netSize();
    Graph_Node get_nth_node(int n);
    vector<Graph_Node>::iterator search_node(const string& val_name);
    void initialise();
    float getProb(int i, string val, vector<string> parent_vals);
    float markovBlanket(int i, string val, vector<string> record);
    vector<string> fill(int i, vector<string> record);
    int retVal(vector<int> Pval, vector<int> PvalSize);
    int index(string value, Graph_Node node);
    float smoothingCoeff(int n);
    float updateCPT(vector<vector<vector<string>>>& records);
    
};
#endif
