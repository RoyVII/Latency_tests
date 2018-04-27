#!/bin/bash
sysbench --test=cpu --time=5600 --cpu-max-prime=20000 run &
sysbench --test=cpu --time=5600 --cpu-max-prime=20000 run &
sysbench --test=cpu --time=5600 --cpu-max-prime=20000 run &
sysbench --test=cpu --time=5600 --cpu-max-prime=20000 run &
sysbench --test=cpu --time=5600 --cpu-max-prime=20000 run &
sysbench --test=cpu --time=5600 --cpu-max-prime=20000 run &
sysbench --test=cpu --time=5600 --cpu-max-prime=20000 run &
sysbench --test=cpu --time=5600 --cpu-max-prime=20000 run &


sysbench --test=fileio --file-total-size=150G prepare
sysbench fileio --file-total-size=150G --file-test-mode=rndrw  --time=300 --max-requests=0 run