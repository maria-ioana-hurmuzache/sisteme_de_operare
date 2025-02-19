#!/bin/bash

if [ $# -lt 1 ]
then
	echo -n "introduceti primul nr: "
	read n
else
	n=$1
fi

if [ $# -lt 2 ]
then
	read -p "introduceti al doilea nurmar: " m
else
	m=$2
fi

prod=0
for i in `seq 1 $m`
do
	let prod=prod+$n
done

echo "rezultat: $n * $m = $prod"
