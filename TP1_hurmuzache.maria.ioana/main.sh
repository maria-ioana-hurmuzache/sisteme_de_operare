#!/bin/bash

if ! [ -d "./xprocesator" ]
then
	echo "Nu exista directorul xprocesator" >&2
	exit 1
elif ! [ -f "./xprocesator/procesator.sh" ]
then
	echo "Nu exista scriptul procesator.sh" >&2
	exit 1
elif ! [ -x "./xprocesator/procesator.sh" ]
then
	echo "dreptul de executie nu e setat" >&2
	exit 1
fi

if ! [ -d "./validators" ]
then
        echo "Nu exista directorul validators" >&2
        exit 2
elif ! [ -f "./validators/validatorASCII.c" ]
then
	echo "Nu exista programul validatorASCII.c" >&2
	exit 2
elif ! [ -r "./validators/validatorASCII.c" ]
then
	echo "dreptul de citire nu este setat" >&2
	exit 2
fi

if ! [ -d "$1" ]
then
	echo "Ce ati introdus nu este un director valid" >&2
	exit 3
elif ! [ -r "$1" ]
then
	echo "directorul nu poate fi citit" >&2
	exit 3
fi

./xprocesator/procesator.sh "$1"

