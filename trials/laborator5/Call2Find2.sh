#!/bin/bash
### Afișarea primelor n linii de text din fișiere. ###

# Validarea datelor de intrare

if [ $# -lt 2 ]
then
    echo "Utilizare: $0 director număr"
    exit 1  # Fail!
fi

if [ ! -d $1 ]
then
    echo "Primul argument trebuie să fie director."
    exit 2  # Fail!
fi

if [ ! $2 -ge 0 ]
then
    echo "Al doilea argument trebuie să fie un număr pozitiv sau 0."
    exit 3  # Fail!
fi

# Parcurgere recursivă implicită, obținută aici cu ajutorul comenzii find

lista=$(find $1 -type f)     # Aici selectăm fișierele de tip obișnuit, ...

for fis in $lista
do
    if [ -r $fis -a -x $fis ]    # ..., iar aici le selectăm pe cele pentru care avem drept de citire și de execuție.
    then
        echo "Fișierul $fis : primele $2 linii de text sunt următoarele:"
        head -n $2 $fis
    fi
done

exit 0  # Succes!

