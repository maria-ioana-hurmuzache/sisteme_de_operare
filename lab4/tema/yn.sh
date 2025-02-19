#!/bin/bash

if [ $# -eq 0 ]
then
	read -p "introduceti n: " n
else
	n=$1
fi

recursie()
{
	if [ $1 -le 1 ]
	then
		echo 1
	else
		echo $(( $(($1+1))*$(recursie $(($1-1)) ) + $(($1+2)) * $(recursie $(($1-2)) ) ))
	fi
}

rezultat=$(recursie $n)
echo "rezultatul este: $rezultat"
