#!/bin/bash
function compilare()
{
	output = ${1%.c}
	gcc $1 -o $output -Wall
}

function afisare()
{
	echo "Continutul fisierului $1 este:"
	cat $1
}

if [ $# -eq 0 ]
then
	echo "utilizare: $0 director"
	exit 1 #Fail!
fi

if [ ! -d $1 -o ! -r $1 ]
then
	echo "Eroare: primul argument, $1, nu e director, sau nu aveti drept de citire al acestuia"
fi

for f in $(ls -A $1)
do
	case $f in
		*.c ) echo "fisier sursa C: $1/$f" ; compilare $1/$f ;;
		*.txt ) echo "fisier text: $1/$f" ; afisare $1/$f ;;
	esac
done
exit 0
