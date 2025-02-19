#!/bin/bash

if [ $# -eq 0 ]
then
	read -p "Scrie n: " n
else
	n=$1
fi

n1=0
n2=1

for ((i=1; i<$n; i++))
do 
	let aux=$n2
	let n2=$n1+$n2
	let n1=$aux
done

echo "al $n-lea nr din sirul fibonacci este: $n2"

