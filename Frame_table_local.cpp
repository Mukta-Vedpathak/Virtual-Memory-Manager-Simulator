#include<bits/stdc++.h>
using namespace std;

class Frame_table_local {
    vector<uint64_t> frames;
    uint64_t max_frames;
    string policy;
    uint64_t max_local_frames;
    unordered_map<uint64_t, vector<uint64_t>> process_frames;
    unordered_map<uint64_t, queue<uint64_t>> process_fifo;
    unordered_map<uint64_t, list<uint64_t>> process_lru_lists;

public:
    Frame_table_local(uint64_t input_max_frames, int max_loc_frames, string replacement_policy) {
        max_frames = input_max_frames;
        for(uint64_t i = 0; i < max_frames; i++) {
            frames.push_back(-1);
        }
        max_local_frames = max_loc_frames;
        policy = replacement_policy;
    }

    bool is_free_frame_available(uint64_t process_id) {
        return process_frames[process_id].size() < max_local_frames;
    }

    int allocate_frame(int process_id) {
        if(process_frames[process_id].size() < max_local_frames) {
            for(uint64_t i = 0; i < max_frames; i++) {
                if(frames[i] == -1) {
                    frames[i] = process_id;
                    process_frames[process_id].push_back(i);
                    if(policy == "FIFO") {
                        process_fifo[process_id].push(i);
                    }
                    if(policy == "LRU") {
                        process_lru_lists[process_id].remove(i);
                        process_lru_lists[process_id].push_back(i);
                    }
                    return i;
                }
            }
        }
        return -1;
    }

    void update_frames(int process_id, int frame_index) {
        if(policy == "LRU") {
            process_lru_lists[process_id].remove(frame_index);
            process_lru_lists[process_id].push_back(frame_index);
        }
    }

    pair<uint64_t, uint64_t> free_the_frame(vector<uint64_t> future_process_ids, int index, int process_id) {
        if(policy == "FIFO") {
            uint64_t frame_to_evict = process_fifo[process_id].front();
            process_fifo[process_id].pop();
            process_frames[process_id].erase(remove(process_frames[process_id].begin(), process_frames[process_id].end(), frame_to_evict), process_frames[process_id].end());
            int pid_of_evicted_process = frames[frame_to_evict];
            frames[frame_to_evict] = -1;
            return {frame_to_evict, pid_of_evicted_process};
        }
        else if(policy == "LRU") {
            if(process_lru_lists[process_id].empty()) {
                return {-1, -1};
            }
            uint64_t frame_to_evict = process_lru_lists[process_id].front();
            uint64_t pid_of_evicted_process = frames[frame_to_evict];
            process_lru_lists[process_id].pop_front();
            process_frames[process_id].erase(remove(process_frames[process_id].begin(), process_frames[process_id].end(), frame_to_evict));
            frames[frame_to_evict] = -1;
            return {frame_to_evict, pid_of_evicted_process};
        }
        else if(policy == "RANDOM") {
            int rand_index = rand() % process_frames[process_id].size();
            int frame_to_evict = process_frames[process_id][rand_index];
            process_frames[process_id].erase(process_frames[process_id].begin() + rand_index); 
            frames[frame_to_evict] = -1;
            return {frame_to_evict, process_id};
        }
        else if(policy == "OPTIMAL") {
           pair<uint64_t, uint64_t> optimal_pair = {0, 0}; 
        int max_dist = -1;
        int selected = -1;

        for(int frame_index : process_frames[process_id]) {
            int distance = std::numeric_limits<int>::max();

            for(int i = index + 1; i < future_process_ids.size(); i++) {
                // Only consider future accesses by the same process (local allocation)
                if(future_process_ids[i] == frames[frame_index]) {
                    distance = i - index;
                    break;
                }
            }

            if(distance == std::numeric_limits<int>::max()) {
                optimal_pair = {frame_index, frames[frame_index]};
                break;
            }

            if(distance > max_dist) {
                max_dist = distance;
                selected = frame_index;
            }
        }

        if(optimal_pair.first == 0 && optimal_pair.second == 0 && selected != -1) {
            optimal_pair = {selected, frames[selected]};
        }

        frames[optimal_pair.first] = -1; 
        return optimal_pair;
    }
        return {-1, -1};
    }
};
