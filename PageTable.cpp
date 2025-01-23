#include <iostream>
#include <bits/stdc++.h>
#include <cstdint>

using namespace std;

class PageTable{
    map<uint64_t, uint64_t> table;
    int page_faults;
public:

    PageTable(): page_faults(0) {} 

    void add_entry(uint64_t virtual_address, uint64_t physical_address){
        table[virtual_address] = physical_address;
    }

    bool check_mapping(uint64_t virtual_address){
        if(table.find(virtual_address) != table.end()) return true;
        return false;
    }

    uint64_t get_physical_address(uint64_t virtual_addr){
        return table[virtual_addr];
    }

    void fault_incrementer(){
        page_faults++;
        // cout<<page_faults<<endl;
    }

    int get_page_faults(){
        return page_faults;
    }

    void remove_entry(uint64_t frame_index){
        for(auto it = table.begin(); it != table.end(); it++){
            if(it->second == frame_index){
                table.erase(it);
                return;
            }
        }
    }

};
