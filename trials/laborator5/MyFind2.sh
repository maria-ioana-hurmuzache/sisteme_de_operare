#!/bin/bash
### Calculeaza recursiv numarul de linii si numarul de caractere din toate fisierele normale ###
### dintr-un director dat si din toate subdirectoarele sale.                                 ###
### Afiseaza numarul total de linii si numarul total de caractere calculate.                 ###

if [ $# -eq 0 ]
then
    read -p "Dati numele directorului de start: " numedir
    [ -z $numedir ] && numedir="."   # Dacă utilizatorul introduce cuvântul vid la read (i.e., apasă direct ENTER), vom procesa directorul curent de lucru!
else
    numedir=$1
fi

nr_linii=0
nr_chars=0

function parcurgere_director ()
{
    for fis in $(ls -A $1)
    do
        # Construim calea până la intrarea $fis din directorul $1

        cale=$1/$fis   # Este suficient doar atât, căci argumentul primului apel nu va putea fi cuvântul vid niciodată! (Motivul: a se vedea primul if din script)

        # Procesăm calea respectivă în funcție de tipul ei, i.e. subdirector/fișier obișnuit.

        if [ -d $cale ] ; then
            parcurgere_director $cale    # apelul recursiv
        elif [ -f $cale ] ; then
            rezl=$(wc -l < $cale)
              # sau, dar mai ineficient: 
              #   rezl=$(wc -l $cale | cut -d" " -f1)  ori, echivalent,  rezl=`cat $cale | wc -l`
            let nr_linii+=rezl

            rezc=`wc -c < $cale`
              # sau, dar mai ineficient: 
              #   rezc=`wc -c $cale | cut -d" " -f1`  ori, echivalent,  rezc=$(cat $cale | wc -c)
            let nr_chars+=rezc
        fi
    done
}

parcurgere_director $numedir    # apelul initial
echo "Directorul $numedir contine $nr_linii linii de text si $nr_chars caractere in fisierele normale aflate in el sau in subdirectoarele sale."

