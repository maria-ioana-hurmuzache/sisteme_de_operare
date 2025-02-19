#!/bin/bash

if [ $# -lt 1 ]
then
	read -p "introduceti n: " n
else
	n=$1
fi

function putere()
{
	p=$1
	if [ $p -eq 0 ]
	then
		echo 1
	else
		let k=$( putere $(($p-1)))
		let m=$((2*$k))
		echo $m
	fi
}

for i in `seq 0 $n`
do
	rez=$( putere $i)
	echo $rez >&2
	#echo $rez
done
