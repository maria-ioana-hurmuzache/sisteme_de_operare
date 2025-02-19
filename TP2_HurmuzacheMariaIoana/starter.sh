#!/bin/bash

if [ $# -ne 1 ]
then
	echo "ai introdus mai mult de un parametru"
	exit 1
fi

if [ "$1" -e 0 ]
then
	./main_app/procesators/worker2 &
	sleep 1
	./main_app/supervisor ./data/input.txt
fi

if [ "$1" -e 1 ]
then
        ./main_app/supervisor ./data/input.txt &
        sleep 2
        ./main_app/procesators/worker2
fi

cd ../dev/shm
cat shm_open
unlink shm_open
rm ./main_app/procesators/myfifo

