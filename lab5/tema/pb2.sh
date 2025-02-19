#!/bin/bash

if [ $# -eq 0 ]
then
	read -p "introduceti calea directorlui: " director
else
	director="$1"
fi

twc=0 #total word count
lloa=0 #longest line of all

recursie_director()
{
	local dir="$1"
	#echo "Director $dir"

	local total_word_count=0
	local longest_line_of_all=0

	for i in "$dir"/*
	do
		if [ -f "$i" ]
		then
			if file "$i" | grep -q "ASCII text"
			then
				#echo "$i"
			        local word_count=$(wc -w < "$i")
       				local longest_line=$(wc -L < "$i")
				#echo "wc: $word_count, ll: $longest_line"
      				total_word_count=$((total_word_count + word_count))
        			if [ "$longest_line" -gt "$longest_line_of_all" ]
        			then
                			longest_line_of_all="$longest_line"
        			fi
			fi
		elif [ -d "$i" ]
		then
			recursie_director "$i"
		fi
	done
	twc=$((twc+total_word_count))
        if [ "$lloa" -lt "$longest_line_of_all" ]
        then
		lloa="$longest_line_of_all"
        fi
	echo "In directorul "$dir" avem un total de "$twc" cuvinte si lungimea maxima a unei linii este "$lloa"."
}

recursie_director "$director"
