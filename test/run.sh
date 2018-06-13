#!/bin/sh

##### Common configurations #####
export LD_LIBRARY_PATH=/g/g90/sato5/opt/opt-catalyst/lib64:$LD_LIBRARY_PATH
export MAT_DIR=./mat_trace

##### Test #####
export OMP_NUM_THREADS=1
time -p ./mat_test_simple_mat
exit
export OMP_NUM_THREADS=1
time -p ./mat_test_stream_mat
exit





