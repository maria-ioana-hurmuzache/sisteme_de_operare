#!/bin/bash

if ! [ -d ./legal_arts ]
then
	echo "Direcrorul legal_arts nu exista" >&2
	exit 1
elif ! [ -f ./legal_arts/iwashere.sh ]
then
	echo "Script-ul iwashere.sh nu exista" >&2
	exit 1
fi

if ! [ -x ./legal_arts/iwashere.sh ]
then
	chmod a+x ./legal_arts/iwashere.sh
fi

if ! [ -d ./legal_arts/performances ]
then
	echo "Directorul performances nu exista" >&2
	exit 2
elif ! [ -f ./legal_arts/performances/filegraffiti.c ]
then
	echo "Fisierul filegraffiti.c nu exista" >&2
	exit 2
elif ! [ -r ./legal_arts/performances/filegraffiti.c ]
then
	echo "Fisierul filegraffiti.c nu poate fi citit" >&2
	exit 2
elif ! [ -f ./legal_arts/performances/filegraffiti ]
then
	gcc -Werror ./legal_arts/performances/filegraffiti.c -o filegraffiti
	if [ "$?" -ne 0 ]
	then
		echo "Eroare la compilare" >&2
		exit 3
	fi
fi

if ! [ -d "$1" ]
then
	echo "Argumentul introdus nu este un director valid" >&2
	exit 4
elif ! [ -r "$1" ]
then
	echo "Directorul introdus nu poate fi citit" >&2
	exit 4
fi

./legal_arts/iwashere.sh "$1" | sort -r | uniq | tr -d '/'

#sort -r ordoneaza descrescator
# tr -d '/' elimina aparitiile caracterului specificat; tr vine de la translate

exit 0
