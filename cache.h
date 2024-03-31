#include <iostream>
#include <bitset>
#include <stdio.h>
#include<stdlib.h>
#include <string>
using namespace std;

#define L1_CACHE_SETS 16
#define L2_CACHE_SETS 16
#define L2_CACHE_WAYS 8
#define MEM_SIZE 4096
#define BLOCK_SIZE 4 // bytes per block
#define DM 0
#define SA 1

struct cacheBlock
{
	int tag; // you need to compute offset and index to find the tag.
	int lru_position; // for SA only
	int data; // the actual data stored in the cache/memory
	bool valid;
	// add more things here if needed
};

struct Stat
{
	int missL1; 
	int missL2; 
	int accL1;
	int accL2;
	// add more stat if needed. Don't forget to initialize!
    int accMain;
    int hitL1;
    int hitL2;
};

class cache {
private:
	cacheBlock L1[L1_CACHE_SETS]; // 1 set per row.
	cacheBlock L2[L2_CACHE_SETS][L2_CACHE_WAYS]; // x ways per row 
	Stat myStat;
	// add more things here
public:
	cache();
	void controller(bool MemR, bool MemW, int* data, int adr, int* myMem);
	// add more functions here ...
    bool search_Cache(int ind, int t, bool& y_L2);
    void update_Cache(int ind, int t);
    void evict_L1_up(int ind, int t, int tosave);
    void store_L2(int ind, int t, int* data);
    int return_L1miss() { return myStat.missL1; }
    int return_L2miss() { return myStat.missL2; }
    int return_acc1() { return myStat.accL1; }
    int return_acc2() { return myStat.accL2; }
    int return_accM() { return myStat.accMain; }
    int return_hit1() { return myStat.hitL1; }
    int return_hit2() { return myStat.hitL2; }
};


