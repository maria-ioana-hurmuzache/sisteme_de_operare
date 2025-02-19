#!/bin/bash

secventa=("$@")
if [ ${#secventa[@]} -eq 0 ]
then
	read -p "insereaza secventa de numere: " -a secventa
fi

secventa101=""
for nr in "${secventa[@]}"
do
	if ((nr>100 | bc -l))
	then
		secventa101+=" $nr"
	fi
done
export secventa101
echo "Consumer: computed sum is $(./consumer.sh "secventa101")"
echo "Producer: consumer's exit code is $?"

