# Compiler
CXX = g++

# Source files and objects
SRC_GLOBAL = code.cpp Frame_table.cpp PageTable.cpp
SRC_LOCAL = code.cpp Frame_table_local.cpp PageTable.cpp

# Executable names
TARGET_GLOBAL = global
TARGET_LOCAL = local

# Default target
all: $(TARGET_GLOBAL) $(TARGET_LOCAL)

# Rule for GLOBAL
$(TARGET_GLOBAL): $(SRC_GLOBAL)
	$(CXX) $(SRC_GLOBAL) -o $(TARGET_GLOBAL)

# Rule for LOCAL
$(TARGET_LOCAL): $(SRC_LOCAL)
	$(CXX) $(SRC_LOCAL) -o $(TARGET_LOCAL)

# Run the simulator with command-line arguments
# Usage example:
# make run_global ARGS="<page-size> <number-of-memory-frames> <replacement-policy> <allocation-policy> <path-to-memory-trace-file>"
run_global: $(TARGET_GLOBAL)
	./$(TARGET_GLOBAL) $(ARGS)

run_local: $(TARGET_LOCAL)
	./$(TARGET_LOCAL) $(ARGS)

# Clean up
clean:
	rm -f $(TARGET_GLOBAL) $(TARGET_LOCAL)

.PHONY: all clean run_global run_local
