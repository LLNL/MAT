#!/bin/sh

##### Common configurations #####
export LD_LIBRARY_PATH=/g/g90/sato5/opt/opt-catalyst/lib64:$LD_LIBRARY_PATH


##### Test #####
## MAT-LLVM simple
export OMP_NUM_THREADS=1
export MAT_DIR=./mat_llvm_trace/
time -p ./mat_test_simple_mat
exit
##


## MAT-PIN simple
export MAT_DIR=./mat_pin_trace/
export OMP_NUM_THREADS=1
pin_dir=/g/g90/sato5/sandbox/pin/pin-3.5-97503-gac534ca30-gcc-linux/
mat_library=/g/g90/sato5/repo/MAT/obj-intel64/mat.so
time -p ${pin_dir}/pin -t ${mat_library}  -- ./mat_test_simple_mat_pin
exit
##






## MAT-LLVM stream
export OMP_NUM_THREADS=1
export MAT_DIR=./mat_llvm_trace/
time -p ./mat_test_stream_mat
exit
##


## MAT-PIN stream
export MAT_DIR=./mat_pin_trace/
export OMP_NUM_THREADS=1
pin_dir=/g/g90/sato5/sandbox/pin/pin-3.5-97503-gac534ca30-gcc-linux/
mat_library=/g/g90/sato5/repo/MAT/obj-intel64/mat.so
time -p ${pin_dir}/pin -t ${mat_library}  -- ./mat_test_stream_mat_pin
exit
##












