#!/bin/bash

if [ $# -eq 0 ]
then
    read -p "Introduceți N:" N
    suma=0
    echo "Introduceți cele $N numere: "
    for ((i=1; i<=N; i++))
    do
        read nr
        suma=$((suma + nr))
    done
    echo "Suma celor $N numere este: $suma"
else
    N=$1
    shift 1
    
    suma=0
    
    while [ $# -gt 0 ]
    do
        for ((i=1; i<=N && $#>0; i++))
	do
            suma=$((suma + $1))
            shift 1
        done
        echo "Suma celor $((i-1)) argumente este: $suma"
        echo "Suma celor $((i-1)) argumente este: $suma" >> output.txt
        suma=0
    done
fi


