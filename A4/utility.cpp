#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <unordered_map>
#include <bits/stdc++.h>

#include "utility.hpp"

using namespace std;

unordered_map<string, int> string_to_idx;
unordered_map<int, string> idx_to_string;


    float randRange(float a, float b) {
        srand(time(NULL));
        // Generate a random number in the range [0, 1)
        float num = ((float)rand()) / (float)RAND_MAX;
        // Scaling and shifting the number to the desired range [a, b]
        return (a + num * (b - a + 1));
    }

    Graph_Node::Graph_Node(string name, int n, vector<string> vals){
        Node_Name = name;
        nvalues = n;
        values = vals;
    }
    string Graph_Node::get_name(){
        return Node_Name;
    }
    vector<int> Graph_Node::get_children(){
        return Children;
    }
    vector<string> Graph_Node::get_Parents(){
        return Parents;
    }
    vector<float> Graph_Node::get_CPT(){
        return CPT;
    }
    int Graph_Node::get_nvalues(){
        return nvalues;
    }
    vector<string> Graph_Node::get_values(){
        return values;
    }
    void Graph_Node::set_CPT(vector<float> new_CPT){
        CPT.clear();
        CPT=new_CPT;
    }
    void Graph_Node::set_Parents(vector<string> Parent_Nodes){
        Parents.clear();
        Parents=Parent_Nodes;
    }

    int Graph_Node::add_child(int new_child_index){
        for(int i=0; i<Children.size();i++){
            if(Children[i]==new_child_index){
                return 0;
            }
        }
        Children.push_back(new_child_index);
        return 1;
    }

    void Graph_Node::random(){
        //Assigning random CPT's
        int cpt_size = CPT.size();
        int parent_combinations = CPT.size() / nvalues;
        vector<float> new_cpt(cpt_size, -1);

        for(int i=0; i < parent_combinations ; i++){
            float sum = 1;
            for(int j = 0; j < nvalues; j++){
                if (j==nvalues-1){
                    new_cpt[j*parent_combinations+i] = sum;
                    break;
                }
                float k = randRange(0,sum);
                new_cpt[j*parent_combinations+i] = k;
                sum = sum-k;
            }
        }
        CPT = new_cpt;
    }


    //The network is represenred as a vector of nodes
    int network::addNode(Graph_Node node){
        Pres_Graph.push_back(node);
        return 0;
    }
    int network::netSize(){
        return Pres_Graph.size();
    }

    //getting the node at nth index
    Graph_Node network::get_nth_node(int n){
        return Pres_Graph[n];
    }

    //getting the iterator of a node with a given name
    vector<Graph_Node>::iterator network::search_node(const string& val_name){
        vector<Graph_Node>::iterator listIt;
        for(listIt=Pres_Graph.begin();listIt!=Pres_Graph.end();listIt++){
            if(listIt->get_name().compare(val_name)==0){
                return listIt;
            }
        }
        cout<<"node not found\n";
        return listIt;
    }

    //Creating CPT randomly
    void network::initialise(){
       vector<Graph_Node>::iterator listIt;
        for(listIt=Pres_Graph.begin();listIt!=Pres_Graph.end();listIt++){
            listIt->random();
        } 
    }

    float network::getProb(int i, string val, vector<string> parent_vals){
        Graph_Node it = get_nth_node(i);
        vector<float> cpt = it.get_CPT();

        if(parent_vals.size()==0){
            vector<string> temp_val = it.get_values();
            for(int k=0; k< temp_val.size(); k++){
                if (temp_val[k] == val){
                    return cpt[k];
                }
            }
            cout << "Error, No matching value found." << endl;
        }

        vector<string> parents = it.get_Parents();
        int n = parent_vals.size();
        int idx = 0;
        int base = 1;
        for(int j = n-1; j>=0;j--){
            Graph_Node temp = get_nth_node(string_to_idx[parents[j]]);
            vector<string> temp_val = temp.get_values();
            for(int k=0; k<temp_val.size();k++){
                if(temp_val[k]==parent_vals[j]){
                    idx = idx + k*base;
                }
            }
            base = base * temp_val.size();
        }

        vector<string> temp_val = it.get_values();
        for(int k=0; k < temp_val.size(); k++){
            if(temp_val[k] == val){
                idx = idx + k*base;
            }
        }
        return cpt[idx];
    }

    float network::markovBlanket(int i, string val, vector<string> record){
        Graph_Node it = get_nth_node(i);
        string name = it.get_name();
        vector<string> parents = it.get_Parents();
        vector<int> children = it.get_children();

        float ans = 1;
        vector<string> temp_values(parents.size(),"");

        for (int j = 0; j<parents.size();j++){
            temp_values[j] = record[string_to_idx[parents[j]]]; 
        }


        ans = ans * getProb(i, val, temp_values);

        for (int j = 0; j < children.size(); j++){
			Graph_Node child_node = get_nth_node(children[j]);
			vector<string> parents_of_child = child_node.get_Parents();
			vector<string> temp_values(parents_of_child.size(),"");
			for (int k = 0; k < parents_of_child.size(); k++){
				if (parents_of_child[k] == val){
					temp_values[k] = val;
					continue;
				}
				temp_values[k] = record[string_to_idx[parents_of_child[k]]];
			}
			ans = ans * getProb(children[j], record[children[j]], temp_values);
		}

		// cout << "Markov ans: " << ans << endl; 
		return ans;
    }

    //Filling the records with appropriate value
	vector<string> network::fill(int i, vector<string> record){
		Graph_Node it = get_nth_node(i);
		vector<string> possible_vals = it.get_values();
		vector<float> prob_values(possible_vals.size(), -1);
		
		float sum = 0;
		for (int j = 0; j<possible_vals.size(); j++){
			prob_values[j] = markovBlanket(i,possible_vals[j],record);
			// cout << prob_values[j] << ":" << possible_vals[j] << endl;
			sum = sum + prob_values[j];
		}
		float maxim = 0;
		int k=0;
		for (int j = 0; j<possible_vals.size(); j++){
			prob_values[j] = prob_values[j]/sum;
			if(prob_values[j]>maxim){
				maxim = prob_values[j];
				k = j;
			}
		}
		
		record[i] = possible_vals[k];
		return record;
	}

    // to convert values to parents to single number to fill in cpt
	int network::retVal(vector<int> Pval, vector<int> PvalSize){
		int n = Pval.size();
		int ret=0;
		int base=1;
		for(int i=n-1; i>-1; i--){
			ret += base*Pval[i];
			base = base * PvalSize[i];
		}
		return ret;
	}

    int network::index(string value, Graph_Node node){
		vector<string> val = node.get_values();
		for(int i=0; i<val.size(); i++){
			if(val[i]==value)
				return i;
		}
		return -1;
	}

	float network::smoothingCoeff(int n) {
    return 0.01 / sqrt(static_cast<float>(n));
    }



	float network::updateCPT(vector<vector<vector<string>>>& records) {
    float abs_diff = 0;

    for (auto listIt = Pres_Graph.begin(); listIt != Pres_Graph.end(); listIt++) {
        int nodeNum = string_to_idx[listIt->get_name()];
        int nVal = listIt->get_nvalues();
        vector<string> parentsT = listIt->get_Parents();
        vector<int> parents;
        vector<int> PvalSize;
        int n = parentsT.size();
        int combinations = 1;

        for (const string& parentName : parentsT) {
            int parentIdx = string_to_idx[parentName];
            int temp = get_nth_node(parentIdx).get_nvalues();
            parents.push_back(parentIdx);
            PvalSize.push_back(temp);
            combinations *= temp;
        }

        vector<float> cptNew;
        vector<vector<int>> storeCount(nVal, vector<int>(combinations, 0));

        for (int j = 0; j < records.size(); j++) {
            for (int k = 0; k < records[j].size(); k++) {
                int value = 0; // value number of the particular node
                string x = records[j][k][nodeNum];
                vector<int> v;
                for (int l = 0; l < n; l++) {
                    string s = records[j][k][parents[l]];
                    v.push_back(index(s, get_nth_node(parents[l])));
                }
                value = retVal(v, PvalSize);
                int varVal = index(x, *listIt);
                storeCount[varVal][value]++;
            }
        }

        vector<int> totalCount(combinations, 0);

        for (int i = 0; i < combinations; i++) {
            for (int j = 0; j < nVal; j++) {
                totalCount[i] += storeCount[j][i];
            }
        }

        for (int i = 0; i < nVal; i++) {
            for (int j = 0; j < combinations; j++) {
                if (totalCount[j] == 0) {
                    totalCount[j] = 1;
                }
                float sCoef = smoothingCoeff(totalCount[j]);
                float totalCountJ = static_cast<float>(totalCount[j]);
				float numerator = static_cast<float>(storeCount[i][j]) + sCoef;
				float denominator = totalCountJ + static_cast<float>(nVal) * sCoef;
				float f = numerator / denominator;
                cptNew.push_back(f);
            }
        }

        const vector<float>& cptOld = listIt->get_CPT();
        for (int i = 0; i < cptNew.size(); i++) {
            abs_diff += abs(cptOld[i] - cptNew[i]);
        }
        listIt->set_CPT(cptNew);
    }
    return abs_diff;
    }