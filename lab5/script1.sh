#!/bin/bash

if [ $# -ne 1 ]
then
    echo "Trebuie sa introduceti un director ca parametru"
    exit 1
fi

if [ ! -d "$1" ] #verific daca e director 
then
    echo "Eroare: $1 nu este un director valid." >&2
    exit 1
fi

if [ ! -r "$1" ] #verific daca are permisiune de citire
then
    echo "Eroare: Nu aveți permisiunea de citire asupra directorului $1." >&2
    exit 2
fi

script2="./subdir_sh/script2.sh"

"$script2" "$1" #apelare

exit_code=$?
echo "Codul de ieșire al celui de-al doilea script: $exit_code"
exit $exit_code
