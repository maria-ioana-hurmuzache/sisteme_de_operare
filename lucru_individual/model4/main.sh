#!/bin/bash

if [ $# -ne 1 ]
then
	echo "Programul necesita un singur argument!\n" >&2
	exit 1
elif ! [ -d "$1" ]
then
	echo "Argumentul primit nu este un director!\n" >&2
	exit 1
fi

if ! [ -d ./scripts ]
then
	echo "Nu exista directorul scripts!\n" >&2
	exit 2
elif ! [ -e ./scripts/listdir.sh ]
then
	echo "Nu exista script-ul listdir.sh!\n" >&2
	exit 2
elif ! [ -x ./scripts/listdir.sh ]
then
	chmod a+x ./scripts/listdir.sh
fi

if ! [ -d ./programs ]
then
	echo "Nu exista directorul programs!\n" >&2
	exit 3
elif ! [ -e ./programs/alphanumeric.c ]
then
	echo "Nu exista programul alphanumeric.c!\n" >&2
	exit 3
elif ! [ -e ./programs/alphanumeric ]
then
	gcc -Werror ./programs/alphanumeric.c -o alphanumeric
	if [ "$?" -ne 0 ]
	then
		echo "Eroare la compilare!\n" >&2
		exit 3
	fi
elif [[ $(stat -c %Y ./programs/alphanumeric.c) -gt $(stat -c %Y ./programs/alphanumeric) ]]
then
	gcc -Werror ./programs/alphanumeric.c -o alphanumeric
        if [ "$?" -ne 0 ]
        then
                echo "Eroare la compilare!\n" >&2
                exit 3
        fi
fi

./scripts/listdir.sh "$1" | head -n 5 | cut -d ':' -f 1

