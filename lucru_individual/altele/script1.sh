#!/bin/bash

if ! [ -e "./rezumat.txt" ]
then
	touch rezumat.txt
fi

rezumat="./rezumat.txt"

>"$rezumat"

#while [ $# -gt 0 ]; do
#   file="$1" # Primul argument
#   shift

nrFiles=0;

for file in "$@"
do
	if ! [ -f "$file" ]
	then
		continue
	fi
	((nrFiles++))
	size=$(stat -c %s "$file")
	last_modified=$(stat -c %y "$file")

	echo "Nume fisier: "$file" , dimensiune $size, data ultimei modificari, "$last_modified" " >>"$rezumat"
done

echo "$nrFiles" >>"$rezumat";
