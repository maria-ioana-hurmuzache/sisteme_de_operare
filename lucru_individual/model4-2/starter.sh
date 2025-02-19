#!/bin/bash

./app/components/worker2 &

sleep 2

./app/worker1 input.txt
