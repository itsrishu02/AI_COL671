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

network read_network(string argv1){
	network Alarm_new;
	string line;
	int find=0;
	ifstream myfile(argv1); 
	string temp;
	string name;
	vector<string> values;
	
	if (myfile.is_open())
	{
		int count = 0;
		while (! myfile.eof() )
		{
			stringstream ss;
			getline (myfile,line);
			
			ss.str(line);
			ss>>temp;
			
			if(temp.compare("variable")==0)
			{
				ss>>name;
				getline (myfile,line);
				stringstream ss2;
				ss2.str(line);
				for(int i=0;i<4;i++)
				{
					ss2>>temp;
				}
				values.clear();
				while(temp.compare("};")!=0)
				{
					temp = temp.substr(1,temp.length()-2);
					values.push_back(temp);
					ss2>>temp;
				}
				name = name.substr(1,name.length()-2);
				string_to_idx[name] = count;
				idx_to_string[count] = name;
				count++;
				Graph_Node new_node(name,values.size(),values);
				int pos=Alarm_new.addNode(new_node);
			}
			else if(temp.compare("probability")==0)
			{
				ss>>temp;
				ss>>temp;
				temp = temp.substr(1,temp.length()-2);
				vector<Graph_Node>::iterator listIt;
				vector<Graph_Node>::iterator listIt1;
				listIt=Alarm_new.search_node(temp);
				int index=string_to_idx[temp];
				ss>>temp;
				values.clear();
				while(temp.compare(")")!=0)
				{
					temp = temp.substr(1,temp.length()-2);
					listIt1=Alarm_new.search_node(temp);
					listIt1->add_child(index);
					values.push_back(temp);
					ss>>temp;
				}
				listIt->set_Parents(values);

				getline (myfile,line);
				stringstream ss2;
				
				ss2.str(line);
				ss2>> temp;
				ss2>> temp;
				temp = temp.substr(1,temp.length()-2);
				vector<float> curr_CPT;
				string::size_type sz;
				while(temp.compare(";")!=0)
				{
					curr_CPT.push_back(atof(temp.c_str()));
					ss2>>temp;
				}
				listIt->set_CPT(curr_CPT);
			}
			else{}
		}
		if(find==1)
		myfile.close();
	}
	return Alarm_new;
}

void reading_database(string argv2, int values, vector<vector<vector<string>>>& records ){
    
    int count;
	string temp;

	//Reading records.dat
	ifstream myfile(argv2);
    
	if (myfile.is_open()){
        
		while( !myfile.eof()){
            
			stringstream ss;
			string line;
			vector<string> rec;
			getline (myfile,line);
			ss.str(line);

			count = -1;
			for (int i = 0; i < values;i++){
				ss >> temp;
				temp = temp.substr(1,temp.length()-2);
				// cout << temp << " ";
				if (temp == "?"){
					count = i;
				}
				rec.push_back(temp);
			}
			count = (count == -1) ? 38 : count;
			records[count].push_back(rec);
		}
	}

	myfile.close();

}

void EM_algo(int values, vector<vector<vector<string>>>& records, network& Alarm ){
	srand(time(0));
    clock_t start;
    clock_t end;
	float elapsed = 0;

	float diff = 1;

	// Running till some condition is satisfied
	while (elapsed < 115){
		//Expectation
		//Inference of each variable
        start = clock();
		vector<float> probs(values,0);
		for (int i = 0; i < values; i++){
			for (int j = 0; j < records[i].size(); j++){
				records[i][j] = Alarm.fill(i,records[i][j]);
			}
		}
		//Maximization
		//Using counting to get actual prob values
		diff = Alarm.updateCPT(records);
		// cout << diff << endl;

		if (diff < 0.00005){
			break;
		}

		end = clock();
		start = end - start;
        elapsed += (float)start/CLOCKS_PER_SEC;
	}
}

void result(string argv1, network& Alarm){

    ifstream file(argv1);
	ofstream outfile("solved_alarm.bif");

	outfile<< fixed << setprecision(4);

	if (file.is_open()){
		int count = 0;
		while( !file.eof()){
			stringstream ss;
			string line;
			string temp;
			vector<string> rec;
			getline (file,line);
			ss.str(line);
			ss >> temp;

			if (temp != "probability"){
				// cout << temp;
				if (count != 0){
					outfile<<endl;
				}
				count++;
				outfile << line;
			}
			else{
				outfile << endl;
				outfile << line << endl;
				ss >> temp;
				ss >> temp;
				temp = temp.substr(1,temp.length()-2);
				vector<float> cpt = Alarm.get_nth_node(string_to_idx[temp]).get_CPT();
				outfile << "	table ";
				for (int i = 0; i < cpt.size(); i++){
					outfile << cpt[i] << " ";
				}
				outfile << ";" << endl;
				outfile << "}";
				getline(file,line);
				getline(file,line);
			}
		}
	}
    file.close();
    outfile.close();
}

int main(int argc, char** argv)
{

	string argv1 = string(argv[1]);
	string argv2 = string(argv[2]);
    
    int values = 37;
    vector<vector<string>> v;
    vector<vector<vector<string>>> records(values+1, v);
    network Alarm;
    
	Alarm=read_network(argv1);
	values = Alarm.netSize();
    

    //Reading database(.dat file)
    reading_database(argv2, values, records);

	//Initialisation of network
	Alarm.initialise();

    //Expectation-Maximization Algorithm
    EM_algo(values,records,Alarm);

	//Creating solved_alarm.bif
    result(argv1, Alarm);

    cout<<"Perfect! Hurrah! \n";
}