#!/bin/bash

cd slaves

#./worker1 &
#sleep 1
./worker2 &

#wait
sleep 3


cd ../master

./supervisor ../input.txt