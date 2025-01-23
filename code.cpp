#include <iostream>
#include <bits/stdc++.h>
#include <string.h>
#include "PageTable.cpp"
#include "Frame_table.cpp"
#include "Frame_table_local.cpp"


using namespace std;

struct PairHash {
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2>& pair) const {
        return std::hash<T1>()(pair.first) ^ (std::hash<T2>()(pair.second) << 1);
    }
};

int main(int argc, char* argv[]) {
    if(argc < 5){
        cerr << "The inputs should be : <page-size> <number-of-memory-frames> <replacement-policy> <allocation-policy> <path-to-memory-trace-file>.";
        return -1;
    }

    int page_size = stoi(argv[1]);
    int number_of_memory_frames = stoi(argv[2]);
    string replacement_policy = argv[3];
    string allocation_policy = argv[4];
    string path_to_trace_file = argv[5];
    int offset = (int)log2(page_size);

    cout<<"Running with frames: "<<number_of_memory_frames<<endl;
    cout<<"Replacement Policy: "<<replacement_policy<<endl;
    cout<<"Allocation Policy: "<<allocation_policy<<endl;

    if(allocation_policy == "GLOBAL"){
        vector<pair<int, uint64_t>> future_process;
        unordered_map<pair<int, uint64_t>,int, PairHash> mp;
        vector<uint64_t> future_process_ids;
        int id=0;
        int total_pf=0;

        PageTable page_tables[4];
        
        Frame_table frame_status(number_of_memory_frames, replacement_policy, allocation_policy);

        ifstream open_trace_file(path_to_trace_file);

        if(!open_trace_file.is_open()){
            cerr << "Error opening the file" ;
            return -1;
        }


        string line;

        // Preprocess the file to record future accesses
        while (getline(open_trace_file, line)) {
            stringstream ss(line);
            int process_id;
            uint64_t virtual_address;
            char comma;

            ss >> process_id >> comma >> virtual_address;

            // Add the current time step to the future access list
            future_process.push_back({process_id, virtual_address >> offset});
        }

        for (const auto& p : future_process) {
            if (mp.find(p) == mp.end()) {
                mp[p] = id++;
            }
        }

        // for (const auto& entry : mp) {
        // std::cout << "Pair (" << entry.first.first << ", " << entry.first.second 
        //           << ") -> ID " << entry.second << std::endl;
        // }

        for (const auto& p : future_process) {
            future_process_ids.push_back(mp[p]);
        }

        open_trace_file.clear();
        open_trace_file.seekg(0);
        int index = 0;

                if( replacement_policy == "OPTIMAL"){
                    //cout<<"hi"<<endl;
                    for(int i=0; i<future_process_ids.size(); i++){
                        if (page_tables[future_process[i].first].check_mapping(future_process[i].second))
                        {
                            uint64_t physical_addr = page_tables[future_process[i].first].get_physical_address(future_process[i].second); // index of the frame
                            //frame_status.update_frame_status(physical_addr);
                            //cout << "Process yes " << future_process_ids[i] << " accessed physical address " << physical_addr << endl;   
                            index++;
                        } else {
                            //cout<<"page fault!" << "index: "<<index<<  future_process[i].first <<" "<<future_process[i].second <<endl;
                            page_tables[future_process[i].first].fault_incrementer();

                        if(frame_status.is_free_frame_available()){
                            int free_frame_index = frame_status.allocate_frame(future_process_ids[i]);
                            page_tables[future_process[i].first].add_entry(future_process[i].second, free_frame_index);
                            //cout << "Adding process " << future_process_ids[i] << " to the page table with virtual addr = " << future_process[i].second << " frame index(physical addr) = " << free_frame_index << endl;
                            index++;
                        } else {
                            pair<uint64_t, uint64_t> evicted_frame = frame_status.free_the_frame(future_process_ids, index, future_process_ids[i]); // {index, pid}

                            for (int k = 0; k<future_process_ids.size() ; k++){
                                if(evicted_frame.second==future_process_ids[k]){
                                    page_tables[future_process[k].first].remove_entry(evicted_frame.first);
                                    //cout<<"entry removed: "<<evicted_frame.first<<endl;
                                    break;}}

                            // }
                           // cout << "Removing frame index(physical addr) = " << evicted_frame.first << " having process " << evicted_frame.second << endl;
                            frame_status.allocate_frame(future_process_ids[i]);
                            page_tables[future_process[i].first].add_entry(future_process[i].second, evicted_frame.first);
                            //cout<<"added "<<future_process_ids[i] <<" "  << future_process[i].second<<"at :"<<evicted_frame.first<<endl;
                            // frame_status.update_frame_status(evicted_frame.first);
                            page_tables[future_process[i].first].get_physical_address(future_process[i].second);
                            //cout << "after removing Adding process " << future_process[i].first << " to the page table with virtual addr = " << future_process[i].second << " frame index(physical addr) = " << evicted_frame.first << endl;
                            index++;
                        }
                    }
                    }
                }

                else{
                    
        while (getline(open_trace_file,  line)) {
                
                stringstream ss(line);
                int process_id;
                uint64_t virtual_address;
                char comma;

                ss >> process_id >> comma >> virtual_address;
                virtual_address = virtual_address >> offset;

                if (page_tables[process_id].check_mapping(virtual_address))
                {
                    uint64_t physical_addr = page_tables[process_id].get_physical_address(virtual_address); // index of the frame
                    frame_status.update_frame_status(physical_addr);
                    //cout << "Process " << process_id << " accessed physical address " << physical_addr << endl;   
                    index++;
                } else {
                    page_tables[process_id].fault_incrementer();

                    if(frame_status.is_free_frame_available()){
                        int free_frame_index = frame_status.allocate_frame(process_id);
                        page_tables[process_id].add_entry(virtual_address, free_frame_index);
                        //cout << "Adding process " << process_id << " to the page table with virtual addr = " << virtual_address << " frame index(physical addr) = " << free_frame_index << endl;
                        index++;
                    } else {
                        pair<uint64_t, uint64_t> evicted_frame = frame_status.free_the_frame(future_process_ids, index, process_id); // {index, pid}

                        for (int i = 0; i<4 ; i++){
                            page_tables[i].remove_entry(evicted_frame.first);
                        }
                        //cout << "Removing frame index(physical addr) = " << evicted_frame.first << " having process " << evicted_frame.second << endl;
                        frame_status.allocate_frame(process_id);
                        page_tables[process_id].add_entry(virtual_address, evicted_frame.first);
                        //cout << "Adding process " << process_id << " to the page table with virtual addr = " << virtual_address << " frame index(physical addr) = " << evicted_frame.first << endl;

                    }
                }
                }
                }

        for (int i = 0; i < 4; i++) {
            cout << "Process " << i << " had " << page_tables[i].get_page_faults() << " page faults." << endl;
            total_pf+=page_tables[i].get_page_faults();
        }
        cout<<"Total page faults:"<<total_pf<<endl;
    }
    else if (allocation_policy == "LOCAL"){




        vector<pair<int, uint64_t>> future_process;
        unordered_map<pair<int, uint64_t>,int, PairHash> mp;
        vector<uint64_t> future_process_ids;
        int max_local_frames = number_of_memory_frames / 4;
        int id=0;
        int total_pf=0;

        PageTable page_tables[4];
        
        Frame_table_local frame_status(number_of_memory_frames,max_local_frames, replacement_policy);

        ifstream open_trace_file(path_to_trace_file);

        if(!open_trace_file.is_open()){
            cerr << "Error opening the file" ;
            return -1;
        }

        string line;

        // Preprocess the file to record future accesses
        while (getline(open_trace_file, line)) {
            stringstream ss(line);
            int process_id;
            uint64_t virtual_address;
            char comma;

            ss >> process_id >> comma >> virtual_address;

            // Add the current time step to the future access list
            future_process.push_back({process_id, virtual_address >> offset});
        }

        for (const auto& p : future_process) {
            if (mp.find(p) == mp.end()) {
                mp[p] = id++;
            }
        }

        // for (const auto& entry : mp) {
        // std::cout << "Pair (" << entry.first.first << ", " << entry.first.second 
        //           << ") -> ID " << entry.second << std::endl;
        // }

        for (const auto& p : future_process) {
            future_process_ids.push_back(mp[p]);
        }

        open_trace_file.clear();
        open_trace_file.seekg(0);
        int index = 0;

                if( replacement_policy == "OPTIMAL"){
                    //cout<<"hi"<<endl;
                    for(int i=0; i<future_process_ids.size(); i++){
                        if (page_tables[future_process[i].first].check_mapping(future_process[i].second))
                        {
                            uint64_t physical_addr = page_tables[future_process[i].first].get_physical_address(future_process[i].second); // index of the frame
                            //frame_status.update_frame_status(physical_addr);
                           // cout << "Process yes " << future_process_ids[i] << " accessed physical address " << physical_addr << endl;   
                            index++;
                        } else {
                            //cout<<"page fault!" << "index: "<<index<<  future_process[i].first <<" "<<future_process[i].second <<endl;
                            page_tables[future_process[i].first].fault_incrementer();

                        if(frame_status.is_free_frame_available(future_process[i].first)){
                            int free_frame_index = frame_status.allocate_frame(future_process[i].first);
                            page_tables[future_process[i].first].add_entry(future_process[i].second, free_frame_index);
                            //cout << "Adding process " << future_process_ids[i] << " to the page table with virtual addr = " << future_process[i].second << " frame index(physical addr) = " << free_frame_index << endl;
                            index++;
                        } else {
                            pair<uint64_t, uint64_t> evicted_frame = frame_status.free_the_frame(future_process_ids, index, future_process[i].first); // {index, pid}

                            for (int k = 0; k<future_process_ids.size() ; k++){
                                if(evicted_frame.second==future_process_ids[k]){
                                    page_tables[future_process[k].first].remove_entry(evicted_frame.first);
                                    //cout<<"entry removed: "<<evicted_frame.first<<endl;
                                    break;}}

                            // }
                           // cout << "Removing frame index(physical addr) = " << evicted_frame.first << " having process " << evicted_frame.second << endl;
                            frame_status.allocate_frame(future_process_ids[i]);
                            page_tables[future_process[i].first].add_entry(future_process[i].second, evicted_frame.first);
                           // cout<<"added "<<future_process_ids[i] <<" "  << future_process[i].second<<"at :"<<evicted_frame.first<<endl;
                            // frame_status.update_frame_status(evicted_frame.first);
                            //page_tables[future_process[i].first].get_physical_address(future_process[i].second);
                           // cout << "after removing Adding process " << future_process[i].first << " to the page table with virtual addr = " << future_process[i].second << " frame index(physical addr) = " << evicted_frame.first << endl;
                           index++;
                        }
                    }
                    }
                }

                else{


        while (getline(open_trace_file,  line)) {
            
                stringstream ss(line);
                int process_id;
                uint64_t virtual_address;
                char comma;

                ss >> process_id >> comma >> virtual_address;
                virtual_address = virtual_address >> offset;

                if (page_tables[process_id].check_mapping(virtual_address))
                {
                    uint64_t physical_addr = page_tables[process_id].get_physical_address(virtual_address); // index of the frame
                    frame_status.update_frames(process_id, physical_addr);
                    // cout << "Process " << process_id << " accessed physical address " << physical_addr << endl;   
                    index++;
                } else {
                    page_tables[process_id].fault_incrementer();

                    if(frame_status.is_free_frame_available(process_id)){
                        int free_frame_index = frame_status.allocate_frame(process_id);
                        page_tables[process_id].add_entry(virtual_address, free_frame_index);
                        // cout << "Adding process " << process_id << " to the page table with virtual addr = " << virtual_address << " frame index(physical addr) = " << free_frame_index << endl;
                        index++;
                    } else {
                        pair<uint64_t, uint64_t> evicted_frame = frame_status.free_the_frame(future_process_ids, index, process_id); // {index, pid}
                        page_tables[evicted_frame.second].remove_entry(evicted_frame.first);
                        // for (int i = 0; i<4 ; i++){
                        //     page_tables[i].remove_entry(evicted_frame.first);
                        // }
                        // cout << "Removing frame index(physical addr) = " << evicted_frame.first << " having process " << evicted_frame.second << endl;
                        frame_status.allocate_frame(process_id);
                        page_tables[process_id].add_entry(virtual_address, evicted_frame.first);
                        // cout << "Adding process " << process_id << " to the page table with virtual addr = " << virtual_address << " frame index(physical addr) = " << evicted_frame.first << endl;

                    
                    }
                }
                
            }
        }

        for (int i = 0; i < 4; i++) {
            cout << "Process " << i << " had " << page_tables[i].get_page_faults() << " page faults." << endl;
            total_pf+=page_tables[i].get_page_faults();
        }

        cout<<"Total page faults:"<<total_pf<<endl;
    }

}

