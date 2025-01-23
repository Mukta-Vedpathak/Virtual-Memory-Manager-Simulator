#!/bin/bash

# Output file
file="op.txt"

# Remove the output file if it exists
if [ -e "$file" ]; then
  rm "$file"
fi

# Iterate over allocation policies
for allocation_policy in GLOBAL LOCAL; do
  echo "-------------------allocation_policy: $allocation_policy-------------------" >> "$file"
  
  # Set executable based on allocation policy
  if [ "$allocation_policy" == "GLOBAL" ]; then
    executable="./global"
  else
    executable="./local"
  fi

  # Iterate over ejection (replacement) policies
  for replacement_policy in FIFO LRU RANDOM OPTIMAL; do
    echo "-------------------replacement_policy: $replacement_policy-------------------" >> "$file"
    
    # Iterate over frame numbers
    for ((frame_number=1024; frame_number<=32768; frame_number*=2)); do
      echo "-------------------num_frames: $frame_number-------------------" >> "$file"
      
      # Run the simulator with page size, frame number, ejection policy, allocation policy, and memory trace file
      $executable 4096 "$frame_number" "$replacement_policy" "$allocation_policy" combined.trace >> "$file"
    done
  done
done
