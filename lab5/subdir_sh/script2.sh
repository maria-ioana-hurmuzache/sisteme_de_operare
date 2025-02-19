#!/bin/bash

parcurgere_director() 
{
    local dir="$1"

    for file in "$dir"/*
do
        if [ -d "$file" ]
        then
            parcurgere_director "$file"
        elif [ -f "$file" ]
        then
            if file "$file" | grep -q "C source" #Comanda file determină tipul de fișier și afișează informații despre acesta. -q face ca grep sa nu afiseze nimic, doar returneaza un cod de iesire
            then
                num_lines=$(wc -l < "$file")
                total_lines=$((total_lines + num_lines))
                echo "$file: $num_lines"
            else
                echo "Primele 5 linii din $file:"
                head -n 5 "$file" >&2
            fi
        fi
    done
}

if [ $# -ne 1 ]
then
    echo "Trebuie sa introduceti un director ca parametru"
    exit 1
fi

if [ ! -d "$1" ]
then
    echo "Eroare: $1 nu este un director valid." >&2
    exit 1
fi

total_lines=0
parcurgere_director "$1"

echo "Numărul total de linii de text în programele C: $total_lines"
if [ $total_lines -gt 0 ]
then
    exit 0
else
    exit 1
fi
