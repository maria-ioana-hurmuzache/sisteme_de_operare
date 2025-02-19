#!/bin/bash

if [ $# -ne 1 ]
then
	exit 2
else
	fileName="$1"
fi

utilizator_curent="$USER"

if [ -f "$fileName" ]
then
	./legal_arts/performances/filegraffiti "$fileName" "$utilizator_curent"
elif [ -d "$fileName" ]
then
	for file in "$fileName"/*
	do
		./legal_arts/iwashere.sh "$file"
	done
fi
