SUBDIRS=../src

#AM_CXXFLAGS = -g -rdynamic -fopenmp
#AM_CFLAGS = -g -rdynamic -fopenmp
#AM_CPPFLAGS = -I../src/
#AM_LDFLAGS = -L../src/.libs/

noinst_PROGRAMS = \
	mat_test_stream

mat_test_stream_SOURCES  = mat_test_stream.cpp
mat_test_stream_CXXFLAGS = -Xclang "-load" -Xclang ../src/.libs/libmatir.so  -O2 -g -Wall -fopenmp=libomp -fno-inline -Wno-deprecated -std=c++11
mat_test_stream_LDFLAGS  = -L./ `llvm-config --ldflags` -L../src/.libs/ -lmat


.PHONY: clean_core
clean_core:
	-rm -f *.core
	-rm -f *.btr


