#!/bin/bash
### Afiseaza un arbore cu structura directorului curent de lucru al utilizatorului, sau al altui director specificat ca argument.  ###

function afisare_indentata()
{
# Functia primeste doua argumente: primul este un nume de director, iar al doilea este nivelul curent
# in arbore (folosit pentru a crea textul de indentare utilizat la afisarea arborelui)

    for fis in $(ls -A $1)    # folosim optiunea -A in loc de -a pentru a nu afisa si intrarile implicite . si ..
    do
        # Construim calea până la intrarea $fis din directorul $1

        cale=$1/$fis   # Este suficient doar atât, căci argumentul primului apel nu va putea fi cuvântul vid niciodată! (Motivul: a se vedea ultimul if din script)

        # Procesăm calea respectivă în funcție de tipul ei, i.e. subdirector/altceva.

        if [ -d $cale ]  # dacă-i director, îl afișăm indentat și...
        then
            let nivel=$2+1
            sir_indentare=
            for i in $(seq $nivel) ; do sir_indentare=$sir_indentare"-> " ; done
            echo -e $sir_indentare$cale

            afisare_indentata $cale $nivel  # ... și apelăm recursiv funcția pentru a-i procesa intrările din el.
        fi
    done
}

if test $# -eq 0 ; then numedir=$(pwd) ; else numedir=$1 ; fi

echo "Afisam structura arborelui cu radacina in directorul: $numedir"
afisare_indentata $numedir 0   # apelul initial



