#!/bin/bash

compilare_c()
{
    file_path="$1"
    file_extension="${file_path##*.}"
    file_name=$(basename "$file_path")
    dir_path=$(dirname "$file_path")
    executable_name="${file_name%.*}"

    gcc -o "$dir_path/$executable_name" "$file_path" >/dev/null 2>&1
    if [ $? -eq 0 ]
    then
        echo "$file_path -> Compilat ok"
    else
        echo "$file_path --> Eroare la compilare!"
    fi
}

#start_dir="$HOME"
start_dir="/home/maria/lab4"

if [ $# -eq 0 ]
then
    read -p "Introduceți numărul pentru adâncimea căutării: " depth
else
    depth=$1
fi

files=$(find "$start_dir" -type f -mtime -28)

if [ $? -ne 0 ]
then
    echo "Command find has failed!" >&2
    exit 1
fi

while IFS= read -r file
do
    if [[ "$file" == *.c ]]
    then
        compilare_c "$file"
    else
        modification_date=$(stat -c %y "$file")
        echo "$modification_date $file"
    fi
done <<< "$files"

