#!/bin/bash

if [ $# -eq 0 ]
then
	read -p "introduceti calea directorului: " dir
else
	dir="$1"
fi

for i in "$dir"/*
do
	if [ -f "$i" ]
	then
		nume=$(basename "$i")
		extensie="${nume##*.}"
		nume_fara_extensie="${nume%.*}"

		if [[ "$extensie" == "txt" ]]
		then
			gzip "$i"
		elif [[ "$extensie" == "sh" ]]
		then	
			zip "$dir"/"$nume_fara_extensie".zip "$i"
		fi
	fi
done

echo "am incheiat arhivarea"
