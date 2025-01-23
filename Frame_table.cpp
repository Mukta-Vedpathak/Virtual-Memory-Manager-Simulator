#include <bits/stdc++.h>

using namespace std;

class Frame_table{
    vector<int> frame;
    int max_frames;
    string policy;
    string allocation_policy;
    queue<int> fifo_queue;
    list<int> lru_list;
    unordered_map<int, queue<uint64_t>> local_fifo;
public:

    Frame_table(int input_max_frames, string replacement_policy, string allocat_policy) {
        max_frames = input_max_frames;
        for(int i = 0; i<max_frames; i++){
            frame.push_back(-1);
        }
        policy = replacement_policy;
        allocation_policy = allocat_policy;
    }

    bool is_free_frame_available() {
        for(int i = 0; i<max_frames; i++){
            if(frame[i] == -1) return true;
        }
        return false;
    }

    int allocate_frame(int process_id){
        for(int i = 0; i<max_frames; i++){
            if(frame[i] == -1){
                frame[i] = process_id;
                if(policy == "FIFO"){
                    fifo_queue.push(i);
                }
                else if(policy == "LRU"){
                    lru_list.remove(i);
                    lru_list.push_back(i);
                }
                return i;
            }
        }
        return -1;
    }

    void update_frame_status(uint64_t frame_index){ // physical_addr
        if(policy=="OPTIMAL"){
            frame[frame_index]=-1;
        }
        if(policy == "LRU"){

            lru_list.remove(frame_index);
            lru_list.push_back(frame_index);
        }
    }

    pair<uint64_t, uint64_t> free_the_frame(vector<uint64_t> future_process_ids, int index, int process_id) {
        if(policy == "RANDOM"){
            int frame_to_evict = rand() % max_frames;

            int pid_of_evicted_frame = frame[frame_to_evict];
            frame[frame_to_evict] = -1;
            return {frame_to_evict, pid_of_evicted_frame};
        }
        if(policy == "FIFO"){
            if(fifo_queue.empty()) return {-1, 1};
            int frame_to_evict = fifo_queue.front();
            fifo_queue.pop();

            int pid_of_evicted_frame = frame[frame_to_evict];
            frame[frame_to_evict] = -1;
            return {frame_to_evict, pid_of_evicted_frame};

        }
        else if(policy == "LRU"){
            if(lru_list.empty()) return {-1, 1};
            int frame_to_evict = lru_list.front();
            lru_list.pop_front();

            int pid_of_evicted_frame = frame[frame_to_evict];
            frame[frame_to_evict] = -1;
            return {frame_to_evict, pid_of_evicted_frame};


        }
        else if(policy == "OPTIMAL"){
                pair<uint64_t, uint64_t> optimal_pair = {0, 0}; // Default initialization
                int max_dist = -1;
                int selected = -1;

                // Single loop to determine the frame for eviction
                for (int frame_index = 0; frame_index < frame.size(); frame_index++) {
                    int distance = INT_MAX; // Default to infinity if the frame is not found in future_process_ids

                    // Check future access distance in a single pass
                    for (int i = index + 1; i < future_process_ids.size(); i++) {
                        if (frame[frame_index] == future_process_ids[i]) {
                            distance = i - index; // Calculate distance to next access
                            break;
                        }
                    }

                    // If this frame is not used again, select it immediately
                    if (distance == INT_MAX) {
                        optimal_pair = {frame_index, frame[frame_index]};
                        break;
                    }

                    // Update farthest future use frame if this one has a greater distance
                    if (distance > max_dist) {
                        max_dist = distance;
                        selected = frame_index;
                    }
                }

                // If no unused frame was found, choose the frame with the farthest future use
                if (optimal_pair.first == 0 && optimal_pair.second == 0 && selected != -1) {
                    optimal_pair = {selected, frame[selected]};
                }

                // Mark the selected frame as free
                frame[optimal_pair.first] = -1; // Mark the frame as free for the next process

                return optimal_pair;
            }
    return {-1, -1};


    }

};