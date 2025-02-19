#!/bin/bash

if [ $# -eq 0 ]
then
	read -p "Introduceti n: " n
else
	n=$1
fi

prim()
{
	ok=1
	if [ $1 -le 1 ]
	then
		let ok=0
	fi
	for ((d=2; d*d<=$1 && ok==1; d++))
	do
		if [ $(($1%d)) -eq 0 ]
		then
			let ok=0
		fi
	done
	echo $ok	
}

recursie()
{
	if [ $1 -eq 0 ]
	then
		echo 0
	else
		echo $(( $(recursie $(($1/10))) + $(($(prim $(($1%10)) )* $(($1%10)) )) ))
	fi
}

rezultat=$(recursie $n)
echo "Rezultatul este: $rezultat"
