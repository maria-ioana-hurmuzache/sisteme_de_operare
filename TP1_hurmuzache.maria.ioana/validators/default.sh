#!/bin/bash

if [ $# -ne 1 ]
then
	echo "Scriptul necesita un argument" >&2
	exit 2
fi

if ! [ -f "./validators/validatorASCII" ]
then
	gcc -Wall "./validators/validatorASCII.c" -o validatorASCII
	if [ $? -ne 0 ]
	then
		echo "Eroare la compilare" >&2
		exit 3
	fi
fi

./validators/validatorASCII "$1" | sed 's/:/->/g'
