#!/bin/bash

if [ "$#" -ne 2 ]
then
	read -p "Introduceti N si varianta de program: " N varianta
else
	N="$1"
	varianta="$2"
fi

if ! [ -d "./programs" ]
then
	echo "Directorul programs nu exista" >&2
	exit 2
fi

if ! [ -r "./programs" ]
then
	echo "Programul nu are acces la directorul programs" >&2
	exit 2
fi

if ! [[ -e "./programs/prg-sc3-1.c" ] && [ -e "./programs/prg-sc3-2.c"]]
then
	echo "Cel putin unul dintre cele 2 programe nu exista" >&2
	exit 2
fi

if ! [ -e "./programs/prg-sc3-1" ]
then
	gcc -Werror "./programs/prg-sc3-1.c" -o "prg-sc3-1"
	if [ "$?" -ne 0 ]
	then
		echo "Eroare la compilarea primului program" >&2
		exit 2
	fi
fi

if ! [ -e "./programs/program2" ]
then    
        gcc -Werror ./programs/program2.c -o program2
        if [ "$?" -ne 0 ]
        then
                echo "Eroare la compilarea celui de-al 2-lea program" >&2
                exit 2
        fi      
fi

if ! [ -d "./instructions" ]
then    
        echo "Directorul instructions nu exista" >&2
        exit 3
fi      

if ! [ -r "./instructions" ]
then
        echo "Programul nu are acces la directorul instructions" >&2
        exit 3
fi

if ! [
