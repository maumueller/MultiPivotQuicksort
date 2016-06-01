#!/bin/sh

# list all available algorithms

../build/src/qstest -A list

sleep 10

# make a standard experiment measuring running time, insertion cout, l1 cache misses, l2 cache misses, and branch mispredictions.
# 250 trials per algorithm and input size, input has size 1m to 1g. 

../build/src/qstest -N  -A qsort1is  -A qsort2v1 -A qsort2v2 -A qsort2v3 -a qsort2v5 -a qsort3 -a qsort5 -a qsort7 -a qsort9 -s 1m -S 1g --papi PAPI_TOT_INS,PAPI_L1_TCM,PAPI_L2_TCM,PAPI_BR_MSP -R 250 permutation | tee kt59-running-time.txt &
