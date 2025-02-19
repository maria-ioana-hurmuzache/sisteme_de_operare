#!/bin/bash

if [ $# -eq 0 ]
then
	read -p "introduceti n: " n
else
	n=$1
fi

recursie()
{
	if [ $1 -eq 0 ]
	then
		echo 0
	else
		local p10=1
		local c=$1
		while [ $(($c/10)) -gt 0 ]
		do
			let p10=$p10*10
			let c=$c/10
		done	
		if [ $((9-$1%10)) -gt 0 ]
		then
			echo $(( $(($((9-$1%10))*$p10)) + $(recursie $(($1/10)))))
		else
			echo $(recursie $(($1/10)))
		fi
	fi	
}

rezultat=$(recursie $n)
echo "rezultatul este: $rezultat"
