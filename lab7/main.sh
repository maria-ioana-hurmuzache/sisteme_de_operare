#!/bin/bash

# main.sh

# 1) Get number of instances N and program variant from command line
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <number_of_instances> <program_variant>"
    exit 1
fi

N=$1
variant=$2

# 2) Test existence and access rights of "programs" directory and executables
programs_dir="programs"
if [ ! -d "$programs_dir" ]; then
    echo "Error: Directory $programs_dir not found"
    exit 2
fi

program_no_locks="prg-sc3-no-locks"
program_locks="prg-sc3-locks"

if [ ! -x "$programs_dir/$program_no_locks" ]; then
    echo "Compiling $program_no_locks..."
    gcc -Werror "$programs_dir/$program_no_locks.c" -o "$programs_dir/$program_no_locks"
fi

if [ ! -x "$programs_dir/$program_locks" ]; then
    echo "Compiling $program_locks..."
    gcc -o "$programs_dir/$program_locks" "$programs_dir/$program_locks.c"
fi

# 3) Test existence and access rights of "instructions" directory and instruction files
instructions_dir="instructions"
if [ ! -d "$instructions_dir" ]; then
    echo "Error: Directory $instructions_dir not found"
    exit 3
fi

instruction_files=("$instructions_dir"/*.txt)
if [ "${#instruction_files[@]}" -ne "$N" ]; then
    echo "Error: Number of instruction files does not match the specified number of instances"
    exit 3
fi

for file in "${instruction_files[@]}"; do
    if [ ! -r "$file" ]; then
        echo "Error: Cannot read instruction file $file"
        exit 3
    fi
done

# 4) Test existence and access rights of "database" directory and database file
database_dir="database"
if [ ! -d "$database_dir" ]; then
    echo "Error: Directory $database_dir not found"
    exit 4
fi

database_file="$database_dir/stocuri.bin"
if [ ! -f "$database_file" ]; then
    echo -n "Enter database information (format: product_code quantity): "
    read -r product_code quantity
    printf "%d %.2f\n" "$product_code" "$quantity" > "$database_file"
fi

if [ ! -r "$database_file" ] || [ ! -w "$database_file" ]; then
    echo "Error: Cannot read/write database file $database_file"
    exit 4
fi

# 5) Launch N instances of the specified program variant
echo "Launching $N instances of $variant..."

for ((i = 1; i <= N; i++)); do
    instruction_file="${instruction_files[i-1]}"
    echo "Launching instance $i with instruction file $instruction_file..."
    ./"$programs_dir/$variant" "$database_file" "$instruction_file" "$i" &
done

wait
echo "All instances finished."

