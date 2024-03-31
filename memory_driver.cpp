#include <fstream>
#include <iostream>
#include <cstring>
#include <sstream>
#include <iostream>
#include <vector>
#include "cache.h"
using namespace std;

struct trace
{
	bool MemR; 
	bool MemW; 
	int adr; 
	int data; 
};

/*
Either implement your memory_controller here or use a separate .cpp/.c file for memory_controller and all the other functions inside it (e.g., LW, SW, Search, Evict, etc.)
*/

int main (int argc, char* argv[]) // the program runs like this: ./program <filename> <mode>
{
	// input file (i.e., test.txt)
	string filename = argv[1];
    //string filename = "L2-test.txt";
    //string filename = "mini_debug.txt";
	
	ifstream fin;

	// opening file
	fin.open(filename.c_str());
	if (!fin){ // making sure the file is correctly opened
		cout << "Error opening " << filename << endl;
		exit(1);
	}
	
	// reading the text file
	string line;
	vector<trace> myTrace;
	int TraceSize = 0;
	string s1,s2,s3,s4;
    while( getline(fin,line) )
      	{
            stringstream ss(line);
            getline(ss,s1,','); 
            getline(ss,s2,','); 
            getline(ss,s3,','); 
            getline(ss,s4,',');
            myTrace.push_back(trace()); 
            myTrace[TraceSize].MemR = stoi(s1);
            myTrace[TraceSize].MemW = stoi(s2);
            myTrace[TraceSize].adr = stoi(s3);
            myTrace[TraceSize].data = stoi(s4);
            //cout<<myTrace[TraceSize].MemW << endl;
            TraceSize+=1;
        }


	// Defining cache and stat
    cache myCache;
    int myMem[MEM_SIZE];  //Main Mem

	int traceCounter = 0;
	bool cur_MemR; 
	bool cur_MemW; 
	int cur_adr;
	int cur_data;

	// this is the main loop of the code
	while(traceCounter < TraceSize){
        
		cur_MemR = myTrace[traceCounter].MemR;
		cur_MemW = myTrace[traceCounter].MemW;
		cur_data = myTrace[traceCounter].data;
		cur_adr = myTrace[traceCounter].adr;
		traceCounter += 1;
        
        //cout << cur_MemR << "," << cur_MemW << "," << cur_adr << "," << cur_data << endl;
        
		myCache.controller (cur_MemR, cur_MemW, &cur_data, cur_adr, myMem); // in your memory controller you need to implement your FSM, LW, SW, and MM.
        //cout << endl << endl;
	}
    /*
    cout << "L1miss: " << myCache.return_L1miss() << endl;
    cout << "L1acc: " << myCache.return_acc1() << endl;
    cout << "L1 hit: " << myCache.return_hit1() << endl;
    cout << "L2miss: " << myCache.return_L2miss() << endl;
    cout << "L2acc: " << myCache.return_acc2() << endl;
    cout << "L2 hit: " << myCache.return_hit2() << endl;
    cout << "Main Mem acc: " << myCache.return_accM() << endl;  */
    
	float L1_miss_rate, L2_miss_rate, AAT; 
	//compute the stats here:
	L1_miss_rate=float(myCache.return_L1miss()) / float(myCache.return_acc1());
    L2_miss_rate=float(myCache.return_L2miss()) / float(myCache.return_acc2());
    float missP_L2 = 100;
    float missP_L1 = float(8) + missP_L2 * L2_miss_rate;
    AAT = float(1)+ missP_L1 * L1_miss_rate;
    
    // 40 L1 access(11 hits), 29 L2 access(5 hits): L1missR=0.725, L2missR=0.828

	cout<< "(" << L1_miss_rate<<","<<L2_miss_rate<<","<<AAT<<")"<<endl;

	// closing the file
	fin.close();

	return 0;
}
