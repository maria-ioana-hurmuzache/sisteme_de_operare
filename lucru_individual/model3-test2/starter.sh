#!/bin/bash

cd slaves
./worker2 &

sleep 3

cd ../master

./supervisor ../input.txt
