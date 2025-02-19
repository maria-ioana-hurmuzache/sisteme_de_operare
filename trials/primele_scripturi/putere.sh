#!/bin/bash

if [ $# -eq 0 ]
then
	read -p "introduceti baza: " n
else
	n=$1
fi

if [ $# -lt 2 ]
then
	read -p "introduceti exponentul: " m
else
	m=$2
fi

putere=1

for i in `seq 1 $m`
do
	let putere=putere*$n
done

echo "rezultat: $n ^ $m = $putere"
exit 0;
	
