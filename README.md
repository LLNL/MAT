# MAT: Memory Analysis Tool (Alpha version) #

	Note: This tool is still alpha version.

This tool traces all memory accesses to stack(Static allocation)/heap(Dynamic allocation) on a trace run in a particular hardware and then estimates execution time on given arbitrary hardware configurations for hardware design space exploration.

## Dependencies

* GOTCHA (https://github.com/LLNL/GOTCHA)
* clang/LLVM
  * v4.0.0 or later

## Quick Start ##

### 1. Building MAT

	$ MAT_DIR=<path to installation directory>
	$ ./autogen.sh
	$ ./configure --with-gotcha=<path to GOTCHA directory> --prefix=$MAT_DIR
	$ make
	$ make install

### 2. Tracing and analysing memory access under MAT

	$ cd test
	$ mkdir mat_llvm_trace
	$ ./run.sh #for quick help
	$ ./run.sh 1 #for tracing simple test code
	$ ./run.sh 5 ./mat_llvm_trace/*/trace-0.mat #for computing reuse distance
	$ ./run.sh 6 ./mat_llvm_trace/*/trace-0.mat 0 > trace-0.txt #for printing out memory access in text
	$ less ./trace-0.txt #for checking memory access in text

## Trace Format
### in binary format (trace-0.mat)

	|trace type(int)|id(size_t)|thread id(int)|memory access type(int)|head address of the buffer(void*)|allocated size for the buffer(size_t)|accessed address(void*)|accessed size in bytes(size_t)|# of instructions executed from the last memory access(int)|

### in text format (trace-0.txt)

	<id> <accessed address> <offset> <head address> <is_read> <accessed size in bytes> <allocated size for the buffer> <thread id>


### Contact

* Kento Sato (kento@llnl.gov)
* David Boehme (boehme3@llnl.gov)

### License

MAT is distributed under the terms of the Apache-2.0 license. See LICENSE and NOTICE for details.

SPDX-License-Identifier: Apache-2.0

LLNL-CODE-805662
