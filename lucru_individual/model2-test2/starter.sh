#!/bin/bash

./workers/worker2 &

sleep 3

./manager/supervisor perechi.txt
