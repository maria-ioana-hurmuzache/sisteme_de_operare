#!/bin/bash

suma=0;
for nr in $secventa101
do
	suma=$((suma+nr)) | bc -l
done

if [ "${#secventa101[@]}" -eq 0 ]
then
	echo "NULL"
	exit 1
else
	echo "$suma"
	exit 0
fi

