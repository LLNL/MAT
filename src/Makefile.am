AM_CFLAGS = -fPIC -g -Wall -pthread -std=c++11 -I$(libz_a_INCLUDE) 
AM_CXXFLAGS = $(AM_CFLAGS)

lib_LTLIBRARIES = libmat.la libmatir.la

mat_SOURCES =  \
		mat.cpp \
		mat_reuse_distance.cpp \
		mat_gotcha.cpp \
		mat_alloc_tree.cpp \
		mat_io.cpp \
		mat_config.cpp \
		mat_util.cpp

libmat_la_SOURCES  = $(mat_SOURCES)
libmat_la_LIBADD   = -lgotcha
libmat_la_CXXFLAGS = -lstdc++ `llvm-config --cxxflags` -I$(libgotcha_INCLUDE)
libmat_la_CPPFLAGS = `llvm-config --cppflags` -I$(libgotcha_INCLUDE)
libmat_la_LDFLAGS  = -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@ `llvm-config --ldflags` -L$(libgotcha_LDFLAGS) 
libmat_la_LDLIBS   = `llvm-config --ldlibs` 


matir_SOURCES =  \
		mat_ir_pass.cpp \
		mat_io.cpp \
		mat_config.cpp \
		mat_util.cpp

libmatir_la_SOURCES  = $(matir_SOURCES)
libmatir_la_LIBADD   =
libmatir_la_CXXFLAGS = -lstdc++ `llvm-config --cxxflags`
libmatir_la_CPPFLAGS = `llvm-config --cppflags`
libmatir_la_LDFLAGS  = -version-info @LIB_CURRENT@:@LIB_REVISION@:@LIB_AGE@ `llvm-config --ldflags`
libmatir_la_LDLIBS   = `llvm-config --ldlibs`




