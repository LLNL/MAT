SUBDIRS=../src

#AM_CXXFLAGS = -g -rdynamic -fopenmp
#AM_CFLAGS = -g -rdynamic -fopenmp
#AM_CPPFLAGS = -I../src/
#AM_LDFLAGS = -L../src/.libs/

noinst_PROGRAMS = \
	mat_test_simple_mat \
	mat_test_simple.ll \
	mat_test_stream \
	mat_test_stream_mat \
	mat_test_stream.ll

mat_test_simple_mat_SOURCES  = mat_test_simple.cpp
mat_test_simple_mat_CPPFLAGS = -Xclang -load -Xclang ../src/.libs/libmatir.so 
mat_test_simple_mat_CXXFLAGS = -O2 -g -Wall -fopenmp=libomp -fno-inline -Wno-deprecated -std=c++11
mat_test_simple_mat_LDFLAGS  = -L./ `llvm-config --ldflags` -L../src/.libs/ -lmat -W

mat_test_simple_ll_SOURCES  = mat_test_simple.cpp
mat_test_simple_ll_CPPFLAGS = -Xclang -load -Xclang ../src/.libs/libmatir.so -emit-llvm -S
mat_test_simple_ll_CXXFLAGS = -O2 -g -Wall -fopenmp=libomp -fno-inline -Wno-deprecated -std=c++11
mat_test_simple_ll_LDFLAGS  = -L./ `llvm-config --ldflags` -L../src/.libs/ -lmat  -Wno-error=unused-command-line-argument -emit-llvm -S

mat_test_stream_SOURCES  = mat_test_stream.cpp
mat_test_stream_CPPFLAGS =
mat_test_stream_CXXFLAGS = -O2 -g -Wall -fopenmp=libomp -fno-inline -Wno-deprecated -std=c++11
mat_test_stream_LDFLAGS  = -L./ `llvm-config --ldflags`

mat_test_stream_mat_SOURCES  = mat_test_stream.cpp
mat_test_stream_mat_CPPFLAGS = -Xclang -load -Xclang ../src/.libs/libmatir.so 
mat_test_stream_mat_CXXFLAGS = -O2 -g -Wall -fopenmp=libomp -fno-inline -Wno-deprecated -std=c++11
mat_test_stream_mat_LDFLAGS  = -L./ `llvm-config --ldflags` -L../src/.libs/ -lmat

mat_test_stream_ll_SOURCES  = mat_test_stream.cpp
mat_test_stream_ll_CPPFLAGS = -Xclang -load -Xclang ../src/.libs/libmatir.so -emit-llvm -S
mat_test_stream_ll_CXXFLAGS = -O2 -g -Wall -fopenmp=libomp -fno-inline -Wno-deprecated -std=c++11
mat_test_stream_ll_LDFLAGS  = -L./ `llvm-config --ldflags` -L../src/.libs/ -lmat -emit-llvm -S -Wno-error=unused-command-line-argument


.PHONY: clean_core
clean_core:
	-rm -f *.core
	-rm -f *.btr

.PHONY: clean_test
clean_test:
	-rm $(noinst_PROGRAMS) *.o


