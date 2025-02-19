#!/bin/bash

if [ $# -ne 2 ]
then
    read -p "Introduceti fisierul si sirul text" file text
else
    file="$1"
    text="$2"
fi

if ! [ -f "$file" ]
then
	touch "$file"
fi

echo "$text" >>"$file"
