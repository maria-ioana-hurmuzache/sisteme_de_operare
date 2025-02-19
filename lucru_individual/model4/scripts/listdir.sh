#!/bin/bash

if [ $# -ne 1 ]
then
	echo "Programul necesita un singur argument!\n" >&2
	exit 2
elif ! [ -d "$1" ]
then
	echo "Argumentul primit nu este un director valid!\n" >&2
	exit 2
elif ! [ -r "$1" ]
then
	echo "Directorul nu poate fi citit!\n" >&2
	exit 2
fi

if [ -f ./forbidden.log ]
then
	rm forbidden.log
fi

touch forbidden.log

parcurgere_recursiva()
{
	local director="$1"
	for file in "$director"/*
	do
		if [ -f "$file" ] && [ -r "$file" ]
		then
			./programs/alphanumeric "$file"
		elif [ -d "$file" ]
		then
			parcurgere_recursiva "$file"
		else
			echo "$file\n" >> forbidden.log
		fi
	done
}

parcurgere_recursiva "$1"
