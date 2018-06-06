SUBDIRS=../src

AM_CXXFLAGS = -g -rdynamic -fopenmp
AM_CFLAGS = -g -rdynamic -fopenmp
AM_CPPFLAGS = -I../src/
AM_LDFLAGS = -L../src/.libs/

noinst_PROGRAMS = \
	mat_test_stream

mat_test_stream_SOURCES = mat_test_stream.cpp
#mat_test_stream_LDADD = xxx

.PHONY: clean_core
clean_core:
	-rm -f *.core
	-rm -f *.btr


