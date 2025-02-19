#!/bin/bash

if [ $# -lt 1 ]
then
	read -p "introduceti nr prim: " p
else
	p=$1
fi

let d=p/2;

for i in `seq 2 $d`
do
{
	if [ $((p%i)) -eq 0 ]
	then
		echo "$p nu este prim"
		exit 1
	fi
}
done

#echo "$p este prim"

suma=0

if [ $# -le 1 ]
then
	read -p "nr de nr n=" n
	for i in $(seq 1 $n)
	do
		read nr
		if [ $((nr%p)) -eq 0 ]
		then
			let suma=suma+nr*nr*nr
		fi
	done
else
	n=$#-1
	for p in $(seq 2 $n)
	do
		if [ $((nr%p)) -eq 0 ]
                then
                        let suma=suma+nr*nr*nr
                fi
	done
fi

echo "suma este: $suma"

exit 0


