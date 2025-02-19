#!/bin/bash

INPUT_FILE="test_input.txt"
NUM_WORKERS=4

# Run the supervisor program
./supervisor_sort1 $NUM_WORKERS $INPUT_FILE > output_supervisor.txt

# Run the sort command for comparison
sort -r $INPUT_FILE > output_sort.txt

# Compare the outputs
diff output_supervisor.txt output_sort.txt

if [ $? -eq 0 ]
then
    echo "Test passed!"
else
    echo "Test failed!"
fi

