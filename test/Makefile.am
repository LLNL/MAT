SUBDIRS=../src
SRCDIR =../src

AM_CXXFLAGS = -g -rdynamic -fopenmp -I../src/
AM_CFLAGS = -g -rdynamic -fopenmp -I../src/
AM_CPPFLAGS = -I$(libgotcha_INCLUDE) -I../src/
AM_LDFLAGS = -L$(libgotcha_LDFLAGS) -L../src/.libs/ -lgotcha

noinst_PROGRAMS = \
	mat_test_simple \
	mat_test_simple_mat_pin \
	mat_test_simple_mat \
	mat_test_simple.ll \
	mat_test_stream \
	mat_test_stream_mat_pin \
	mat_test_stream_mat \
	mat_test_stream.ll \
	mat_test_rd

mat_test_simple_SOURCES  = mat_test_simple.cpp
mat_test_simple_CPPFLAGS =
mat_test_simple_CXXFLAGS = -O2 -g -Wall -fopenmp=libomp -fno-inline -Wno-deprecated -std=c++11
mat_test_simple_LDFLAGS  = -L./ `llvm-config --ldflags` -W

mat_test_simple_mat_pin_SOURCES  = mat_test_simple.cpp
mat_test_simple_mat_pin_CPPFLAGS =
mat_test_simple_mat_pin_CXXFLAGS = -O2 -g -Wall -fopenmp=libomp -fno-inline -Wno-deprecated -std=c++11 -DUSE_MAT
mat_test_simple_mat_pin_LDFLAGS  = -L./ `llvm-config --ldflags` -W

mat_test_simple_mat_SOURCES  = mat_test_simple.cpp
mat_test_simple_mat_CPPFLAGS = -Xclang -load -Xclang ../src/.libs/libmatir.so 
mat_test_simple_mat_CXXFLAGS = -O2 -g -Wall -fopenmp=libomp -fno-inline -Wno-deprecated -std=c++11 -DUSE_MAT_LLVM -I../src/
mat_test_simple_mat_LDFLAGS  = -L./ `llvm-config --ldflags` -L../src/.libs/ -lmat -W

mat_test_simple_ll_SOURCES  = mat_test_simple.cpp
mat_test_simple_ll_CPPFLAGS = -Xclang -load -Xclang ../src/.libs/libmatir.so -emit-llvm -S
mat_test_simple_ll_CXXFLAGS = -O2 -g -Wall -fopenmp=libomp -fno-inline -Wno-deprecated -std=c++11
mat_test_simple_ll_LDFLAGS  = -L./ `llvm-config --ldflags` -L../src/.libs/ -lmat  -Wno-error=unused-command-line-argument -emit-llvm -S


STREAM_N=100000
STREAM_NTIMES=10

mat_test_stream_SOURCES  = mat_test_stream.cpp
mat_test_stream_CPPFLAGS = 
mat_test_stream_CXXFLAGS = -O2 -g -Wall -fopenmp=libomp -fno-inline -Wno-deprecated -std=c++11 #-DSTREAM_ARRAY_SIZE=$(STREAM_N) -DNTIMES=$(STREAM_NTIMES)
mat_test_stream_LDFLAGS  = -L./ `llvm-config --ldflags`

mat_test_stream_mat_pin_SOURCES  = mat_test_stream.cpp
mat_test_stream_mat_pin_CPPFLAGS =
mat_test_stream_mat_pin_CXXFLAGS = -O2 -g -Wall -fopenmp=libomp -fno-inline -Wno-deprecated -std=c++11 -DUSE_MAT  -DSTREAM_ARRAY_SIZE=$(STREAM_N) -DNTIMES=$(STREAM_NTIMES)
mat_test_stream_mat_pin_LDFLAGS  = -L./ `llvm-config --ldflags`

mat_test_stream_mat_SOURCES  = mat_test_stream.cpp
mat_test_stream_mat_CPPFLAGS = -Xclang -load -Xclang ../src/.libs/libmatir.so 
mat_test_stream_mat_CXXFLAGS = -O2 -g -Wall -fopenmp=libomp -fno-inline -Wno-deprecated -std=c++11 -I../src/ -DUSE_MAT_LLVM -DSTREAM_ARRAY_SIZE=$(STREAM_N) -DNTIMES=$(STREAM_NTIMES)
mat_test_stream_mat_LDFLAGS  = -L./ `llvm-config --ldflags` -L../src/.libs/ -lmat

mat_test_stream_ll_SOURCES  = mat_test_stream.cpp
mat_test_stream_ll_CPPFLAGS = -Xclang -load -Xclang ../src/.libs/libmatir.so -emit-llvm -S
mat_test_stream_ll_CXXFLAGS = -O2 -g -Wall -fopenmp=libomp -fno-inline -Wno-deprecated -std=c++11  -DSTREAM_ARRAY_SIZE=$(STREAM_N) -DNTIMES=$(STREAM_NTIMES)
mat_test_stream_ll_LDFLAGS  = -L./ `llvm-config --ldflags` -L../src/.libs/ -lmat -emit-llvm -S -Wno-error=unused-command-line-argument

mat_SOURCES =  \
		$(SRCDIR)/mat_reuse_distance.cpp \
                $(SRCDIR)/mat_io.cpp \
                $(SRCDIR)/mat_config.cpp \
                $(SRCDIR)/mat_util.cpp
mat_test_rd_SOURCES  = mat_test_rd.cpp $(mat_SOURCES)
mat_test_rd_CPPFLAGS =
mat_test_rd_CXXFLAGS = -O2 -g -Wall -fopenmp=libomp -fno-inline -Wno-deprecated -std=c++11  -I../src/
mat_test_rd_LDFLAGS  = -L./ `llvm-config --ldflags` -W


.PHONY: clean_core
clean_core:
	-rm -f *.core
	-rm -f *.btr

.PHONY: clean_test
clean_test:
	-rm $(noinst_PROGRAMS) *.o

.PHONY: clean_mat
clean_mat:
	-rm -rf ./mat_llvm_trace/*
	-rm -rf ./mat_pin_trace/*


