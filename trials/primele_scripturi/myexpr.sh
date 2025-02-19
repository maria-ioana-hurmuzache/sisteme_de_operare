#!/bin/bash
### Calculator aritmetic ###

if [ $# -eq 1 ] ; then
    fisier=$1
else
    echo -n "Dati numele fisierului: " ; read fisier
fi

nr_op=0

echo > $fisier    # Sau, echivalent, putem scrie:  truncate -s 0 $fisier

while true
do
    read -p "Dati primul numar: " nr1
    read -p "Dati al doilea numar: " nr2
    echo -n "Dati operatorul: " ; read op

    case "$op" in
        "+" ) let rez=nr1+nr2 ;;
        "-" ) let rez=nr1-nr2 ;;
        "*" ) let rez=nr1*nr2 ;;
        "/" ) rez=$(echo "$nr1 / $nr2" | bc -l) ;;    # Sau, mai putem scrie:  rez=$(echo "scale=5; $nr1 / $nr2" | bc)
        "q" ) break
    esac
    let nr_op++
    echo "$nr_op:  $nr1 $op $nr2 = $rez" >> $fisier
done

echo "Numarul total de operatii efectuate: $nr_op" >> $fisier
