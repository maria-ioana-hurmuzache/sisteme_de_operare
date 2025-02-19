#!/bin/bash

./subordinates/worker1 &
sleep 2
./coordonator/supervisor ./operatii.txt
