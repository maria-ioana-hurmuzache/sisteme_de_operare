#!/bin/bash

if [ $# -eq 0 ]
then
	read -p "Introduceti n: " n
	read -p "Introduceti k: " k
else
	n=$1
	k=$2
fi

produs=1
cn=$n

for ((i=0; i<k; i++))
do
	let produs=$produs*$cn
	let cn=$cn-1
done

echo "Aranjamente de $n luate cate $k = $produs."
