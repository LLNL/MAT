AM_CFLAGS = -fPIC -g -Wall -pthread -std=c++11 -I$(libz_a_INCLUDE) 
AM_CXXFLAGS = $(AM_CFLAGS)

mat_SOURCES =  \
		mat_io.cpp \
		mat_config.cpp \
		mat_util.cpp


lib_LTLIBRARIES = libmat.la
libmat_la_SOURCES  = $(mat_SOURCES)
libmat_la_LIBADD =  $(libz_a_LIB)
libmat_la_CFLAGS = -lz -lstdc++ 
libmat_la_CPPFLAGS = 
libmat_la_LDFLAGS = -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@


