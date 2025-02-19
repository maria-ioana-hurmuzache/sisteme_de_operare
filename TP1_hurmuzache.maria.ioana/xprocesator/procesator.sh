#!/bin/bash

if [ $# -ne 1 ]
then
	echo "Trebuie introdus un argument" >&2
	exit 2
elif ! [ -d "$1" ]
then
	echo "Argumentul primit nu este director" >&2
	exit 3
elif ! [ -r "$1" ]
then
	echo "Directorul nu poate fi citit" >&2
	exit 3
elif ! [ -x "$1" ]
then
	echo "Directorul nu poate fi executat" >&2
	exit 3
fi	

if ! [ -d "./validators" ]
then
	echo "Nu exista directorul validators" >&2
	exit 1
elif ! [ -f "./validators/default.sh" ]
then
	echo "Nu exista scriptul default.sh" >&2
	exit 1
elif ! [ -x "./validators/default.sh" ]
then    
        echo "Nu am drept de executie pe scriptul default.sh" >&2
        exit 1
fi

for file in "$director"/*
do
	if [ -f "$file" ] && [ -r "$file" ]
	then
		./validators/default.sh "$file"
		1>"./result.txt"
		2>"./result.txt"
	elif [ -d "$file" ]
	then
		./xprocesator/procesator.sh "$file"
	fi
done
