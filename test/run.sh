#!/bin/sh

##### Common configurations #####
export LD_LIBRARY_PATH=/g/g90/sato5/opt/opt-catalyst/lib64:$LD_LIBRARY_PATH




##### Test #####


bin=""
case $1 in
    0) ## clean trace
	rm -rf ./mat_llvm_trace/*
	;;
    1) ## MAT-LLVM simple
	export OMP_NUM_THREADS=1
	export MAT_DIR=./mat_llvm_trace/
	time -p ./mat_test_simple_mat
	;;
    2) ## MAT-PIN simple
	export MAT_DIR=./mat_pin_trace/
	export OMP_NUM_THREADS=24
	pin_dir=/g/g90/sato5/sandbox/pin/pin-3.5-97503-gac534ca30-gcc-linux/
	mat_library=/g/g90/sato5/repo/MAT/obj-intel64/mat.so
	time -p ${pin_dir}/pin -t ${mat_library}  -- ./mat_test_simple_mat_pin
	;;
    3) ## MAT-LLVM stream
	export OMP_NUM_THREADS=1
	export MAT_DIR=./mat_llvm_trace/
	time -p ./mat_test_stream_mat
	;;
    4) ## MAT-PIN stream
	export MAT_DIR=./mat_pin_trace/
	export OMP_NUM_THREADS=1
	pin_dir=/g/g90/sato5/sandbox/pin/pin-3.5-97503-gac534ca30-gcc-linux/
	mat_library=/g/g90/sato5/repo/MAT/obj-intel64/mat.so
	time -p ${pin_dir}/pin -t ${mat_library}  -- ./mat_test_stream_mat_pin
	;;
    5) ## MAT reuse distance
	time -p ../src/mat-rd $2
	;;
    6) ## MAT binary to text
	time -p ../src/mat-b2t $2 $3
	;;
    7) ## MAT mgraph
	touch .empty
	time -p ../scripts/mat-mg $2 .empty
	rm .empty
	;;
    *)
	echo "No such test case: $1"
	echo "  0) Clean trace files"
	echo "  1) MAT-LLVM simple"
	echo "  2) MAT-PIN  simple"
	echo "  3) MAT-LLVM stream"
	echo "  4) MAT-PIN  stream"
	echo "  5) MAT reuse distance"
	echo "  6) MAT binary to text"
	echo "  7) MAT memory graph"
	;;
esac
wait
exit










