#include "cache.h"

cache::cache()
{
	for (int i=0; i<L1_CACHE_SETS; i++)
		L1[i].valid = false; 
	for (int i=0; i<L2_CACHE_SETS; i++)
		for (int j=0; j<L2_CACHE_WAYS; j++)
			L2[i][j].valid = false; 

	this->myStat.missL1 =0;
	this->myStat.missL2 =0;
	this->myStat.accL1 =0;
	this->myStat.accL2 =0;
    this->myStat.accMain =0;
    this->myStat.hitL1 =0;
    this->myStat.hitL2 =0;
}
void cache::controller(bool MemR, bool MemW, int* data, int adr, int* myMem)
{
	// add your code here
    // convert integer to binary
    bitset<32> addr = adr;
    bitset<4> index;
    bitset<28> tag;
    // extract index and tag (L1,L2 are same)
    for (int i = 0, j = 0; i < 4 && j < 4; i++, j++)
    {   index[i] = addr[j];   }
    for (int i = 0, j = 4; i < 28 && j < 32; i++, j++)
    {   tag[i] = addr[j];   }
    int c_i = (int)index.to_ulong();
    int c_t = (int)tag.to_ulong();
    
    // check read or write
    if (MemR) {
        // load op
        bool checkL2 = false;
        // check L1 cache
        if (search_Cache(c_i, c_t, checkL2)) {
            // update stats, Update data, tag
            myStat.hitL1 ++;
            
            //cout << "hit L1!" << endl;
            *data = L1[c_i].data;
            
        } else {
            // L1 doesn't find target, check L2 cache
            myStat.missL1 ++;
            
            if (search_Cache(c_i, c_t, checkL2)) {
                // found in L2
                myStat.hitL2 ++;
                
                //cout << "hit L2!" << endl;
                
                // bring data to L1 (and remove it from L2) and update LRU, data, and tag
                update_Cache(c_i, c_t);
                *data = L1[c_i].data;
                
            } else {
                // L1, L2 can't find, bring from main mem
                myStat.missL2 ++;
                *data = myMem[adr];
                // bring to L1, evict L1, evicted data to L2, remove L2
                evict_L1_up(c_i, c_t, myMem[adr]);
                myStat.accMain ++;
            }
        }
    }
    
    // check read or write
    if (MemW) {
        // store op
        bool checkL2 = false;
        // check L1 cache
        if (search_Cache(c_i, c_t, checkL2)) {
            myStat.hitL1 ++;
            
            //cout << "hit L1!" << endl;
            // update data in L1
            evict_L1_up(c_i, c_t, *data);
            
        } else {
            // L1 doesn't find target, check L2 cache
            myStat.missL1 ++;
            if (search_Cache(c_i, c_t, checkL2)) {
                // found in L2
                myStat.hitL2 ++;
                
                //cout << "hit L2!" << endl;
                
                // update data in L2 and its LRU
                store_L2(c_i, c_t, data);
                evict_L1_up(c_i, c_t, *data);
                
            } else {
                // L1, L2 can't find, bring from main mem
                myStat.missL2 ++;
            }
        }
        // need to update main mem in anyway
        myMem[adr] = *data;
        //cout << "saved data: " << myMem[adr] << " at " << adr << endl;
        
        myStat.accMain ++;
    }
    // end of the func
}

// helper function: searching caches
bool cache::search_Cache(int cur_i, int cur_t, bool& y_L2)
{
    bool found = false;
    
    if (!y_L2) {
        // search L1
        myStat.accL1 ++;
        if (L1[cur_i].valid && L1[cur_i].tag == cur_t) {
            // mark true if found in L1
            found = true;   }
        // not found in L1, mark bool var
        y_L2 = true;    }
    else {
        // search L2
        myStat.accL2 ++;
        for (int i = 0; i < L2_CACHE_WAYS; i++) {
            if (L2[cur_i][i].valid && L2[cur_i][i].tag == cur_t) {
                // mark true if found in L2
                found = true;
                break;  }
        }
    }
    return found;
}

// helper function: update L2, when data found in L2
void cache::update_Cache(int ind, int t)
{
    // called when data found in L2
    for(int i = 0; i < L2_CACHE_WAYS; i++) {
        if (L2[ind][i].valid && L2[ind][i].tag == t) {
            int tempD = L2[ind][i].data;
            int tempT = L2[ind][i].tag;
            L2[ind][i].data = L1[ind].data;
            L2[ind][i].tag = L1[ind].tag;
            L1[ind].data = tempD;
            L1[ind].tag = tempT;
            // update LRU in L2
            if (L2[ind][i].lru_position != 7){
                for (int j = 0; j < L2_CACHE_WAYS; j++) {
                    if (L2[ind][j].valid && j != i) {
                        if (L2[ind][j].lru_position > L2[ind][i].lru_position)
                            L2[ind][j].lru_position --;     }
                }
                L2[ind][i].lru_position = 7;    }
        }
    }
    // end of the func
}

// helper function: update caches, when directly read data from main mem
void cache::evict_L1_up(int ind, int t, int tosave)
{
    // update L1 (bring data into)
    int evict_t1 = 0;
    int evict_d1 = 0;
    bool isE = false;
    if(L1[ind].valid && L1[ind].tag != t) {
        evict_t1 = L1[ind].tag;
        evict_d1 = L1[ind].data;
        isE = true; }
    L1[ind].data = tosave;
    L1[ind].tag = t;
    L1[ind].valid = true;
    /*
    cout << "load from L1 [" << ind << "]: " << tosave << endl;
    cout << "Evict data? " << isE << endl;  */
    
    if (isE) {
        // put evicted data into L2, remove original value in L2
        int evict_pos = 0;
        int total_p = 0;
        for (int i = 0; i < L2_CACHE_WAYS; i++) {
            if (L2[ind][i].valid) {
                total_p ++;
                if (L2[ind][i].lru_position == 0) {
                    evict_pos = i;  }   }
        }
        
        if (total_p !=0) {
            // meaning the set is not empty (fully occupied or partially)
            if (total_p == L2_CACHE_WAYS) {
                L2[ind][evict_pos].data = evict_d1;
                L2[ind][evict_pos].tag = evict_t1;
                // update LRU position
                for (int j = 0; j < L2_CACHE_WAYS; j++) {
                    if (L2[ind][j].valid && j != evict_pos) {
                        if (L2[ind][j].lru_position > L2[ind][evict_pos].lru_position)
                            L2[ind][j].lru_position --;
                    }
                }
                L2[ind][evict_pos].lru_position = total_p - 1;
                //L2[ind][evict_pos].valid = true;    // invalid evict position
            }
            else {  // still have empty space
                for (int v = 0; v < L2_CACHE_WAYS; v++) {
                    if (L2[ind][v].valid == false) {
                        L2[ind][v].valid = true;
                        L2[ind][v].data = evict_d1;
                        L2[ind][v].tag = evict_t1;
                        // update LRU position
                        L2[ind][v].lru_position = total_p;
                        break;  }
                }
            }
            
        } else {
            // the set is empty, put here directly
            L2[ind][0].valid = true;
            L2[ind][0].data = evict_d1;
            L2[ind][0].tag = evict_t1;
            L2[ind][0].lru_position = 0;
        }
    }
    // end of the func
}

// helper function: store new data to L2
void cache::store_L2(int ind, int t, int* data)
{
    // update data
    int record = 0;
    int tot = 0;
    for (int i = 0; i < L2_CACHE_WAYS; i++) {
        if (L2[ind][i].valid) {
            tot ++;
            if (L2[ind][i].tag == t) {
                L2[ind][i].data = *data;
                record = i; }
            }
    }
    // update LRU
    for (int k = 0; k < tot; k++) {
        if (k != record && L2[ind][k].lru_position > L2[ind][record].lru_position)
            L2[ind][k].lru_position --;
    }
    L2[ind][record].lru_position = tot - 1;
    // end of the func
}

