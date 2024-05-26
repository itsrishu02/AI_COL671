#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>
using namespace std;


void process_miniSAT_result_to_produce_output(const std::string& input_file, const std::string& output_file, int n, int k1, int k2) {
    std::ifstream infile(input_file);
    std::ofstream outfile(output_file);

    // Read first line and output 0 if it is 'UNSAT'
    std::string first_line;
    std::getline(infile, first_line);
    //std::cout << "Input File Line 1: " << first_line << std::endl;
    if (first_line == "UNSAT") {
        outfile << "0" << std::endl;
        return;
    }

    // For 'SAT' cases, read & store positive variables (sort first k1 and then k2 separately)
    else if (first_line == "SAT") {
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

        outfile << "#2" << std::endl;
        for (int i = k1; i < k1 + k2; ++i) {
            if(i==k1)
            outfile << X_k1[i];
            else outfile << " "<<X_k1[i];
        }
        outfile << std::endl;
    }
}

int main(int argc, char** argv) {
    
    

    string te = argv[1];
    ifstream input( te+".graph" );

    int n, m, k1, k2;
    input >> n >> m >> k1 >> k2;
    vector<std::pair<int, int>> edges;

    string input_file2 = te+".satoutput";
    string output_file = te+".mapping";

    process_miniSAT_result_to_produce_output(input_file2, output_file, n, k1, k2);

    return 0;
}
