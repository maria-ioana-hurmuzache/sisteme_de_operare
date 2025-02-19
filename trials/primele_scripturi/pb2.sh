#!/bin/bash

if [ $# -eq 0 ]
then
       	read -p "n este: " n
else
       	n=$1
fi

function fibonacci()
{
	if [ $1 -le 2 ]
	then
		echo 1
	else
		let n1=$1-1
		let n2=$1-2
		rez=$(($(fibonacci $n1) + $(fibonacci $n2)))
		echo $rez
	fi
}

fibonacci $n
