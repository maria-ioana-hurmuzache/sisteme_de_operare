#!/bin/bash
# Script pentru testarea paralelă a programului prg-sc1

# Resetăm fișierul binar cu o valoare inițială
#echo 50000 > peco.bin

# Lansează mai multe instanțe cu operații diferite
./prg-sc1 25500 -10.9 -35.42 & 
./prg-sc1 -20.45 3500 5000 & 
./prg-sc1 -20000 10000 12000 &
wait

