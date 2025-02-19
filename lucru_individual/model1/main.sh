#!/bin/bash

if ! [ -e "./scripts/listdir.sh" ]
then
	echo "Folder-ul scripts si/sau fisierul listdir.sh nu exista" >&2
	exit 1
elif ! [ -x "./scripts/listdir.sh" ]
then
	echo "Fisierul listdir.sh nu poate fi executat" >&2
	exit 1
fi

if ! [ -e "./programs/filetypeandsize.c" ]
then
	echo "Folder-ul programs si/sau fisierul filetypeandsize.c nu exista" >&2
	exit 2
elif ! [ -r "./programs/filetypeandsize.c" ]
then
	echo "Fisierul filetypeandsize.c nu poate fi executat" >&2
	exit 2
fi

if ! [ -e "./programs/filetypeandsize" ]
then
	gcc -Wall ./programs/filetypeandsize.c -o filetypeandsize
	if [ "$?" -ne 0 ]
	then
		echo "Fisierul filetypeandsize nu exista si nu a putut fi creat" >&2
		exit 3
	fi
fi

if ! [[ -d "$1" && -r "$1" ]]
then
	echo "Directorul nu exista sau nu are permisiunea de a fi citit" >&2
	exit 4
fi

./scripts/listdir.sh "$1" | sed 's/:/---/g' | head -n 20 | tail -n 16

exit 0
