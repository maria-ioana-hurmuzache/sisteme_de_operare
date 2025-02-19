#!/bin/bash

if [ "$#" -ne 1 ]
then
	exit 2
fi

./programs/filetypeandsize "$1"

if [ "$?" -eq 1 ]
then
	for file in "$1"/*
	do
		./scripts/listdir.sh "$file"
	done
fi
