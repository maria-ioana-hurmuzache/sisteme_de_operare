#!/bin/bash

if [ $# -eq 0 ]
then
	read -p "introduceti n: " n
else
	n=$1
fi

recursie()
{
	if [ $1 -eq 0 ]
	then
		echo 1
	elif [ $1 -eq 1 ]
	then
		echo 2
	else
		echo $(( $(recursie $(($1-1)) ) + $((4* $(recursie $(($1-2)) ) )) + $1 ))
	fi
}

rezultat=$(recursie $n)
echo "rezultatul este: $rezultat"
