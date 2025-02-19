#!/bin/bash

if [ $# -eq 0 ]
then
	read -p "Introduceti n: " n
else
	n=$1
fi

produs=1

for ((i=2; i<=n; i++))
do
	let produs=$produs*$i
done

echo "Factorialul numarului $n este $produs."
