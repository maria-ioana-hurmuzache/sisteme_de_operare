#!/bin/bash
### Afișează lista parametrilor cu care este apelat. ###

i=0
for parametru in $*
do
  let i++
  echo "param_$i = $parametru"
done

