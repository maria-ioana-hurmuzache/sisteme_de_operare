#!/bin/bash
### Afișarea comentariilor din script-uri. ###

# Validarea datelor de intrare

if [ $# -eq 0 ]
then
    read -p "Dați directorul de start:" dir
    [ -z $dir ] && dir="."   # Dacă utilizatorul introduce cuvântul vid la read (i.e., apasă direct ENTER), vom procesa directorul curent de lucru!
else
    dir=$1
fi

if [ ! -d $dir -o ! -r $dir ]
then
    echo "Argumentul dat, $dir, nu este director, sau nu aveti drept de citire a lui!"
    exit 1  # Fail!
fi

# Parcurgere recursivă implicită, obținută aici cu ajutorul comenzii find
lista=$(find $dir -type f -name "*.sh")

for fis in $lista
do
    echo $fis
    grep -n '#' $fis | grep -v '$#' | grep -v '#!'
done

exit 0  # Succes!
