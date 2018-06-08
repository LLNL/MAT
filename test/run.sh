#!/bin/sh

export OMP_NUM_THREADS=1
./mat_test_stream
exit
export OMP_NUM_THREADS=1
./mat_test_simple
exit

