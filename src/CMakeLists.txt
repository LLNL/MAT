set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(MAT_SOURCES
    mat.cpp
    mat_reuse_distance.cpp
    mat_gotcha.cpp
    mat_alloc_tree.cpp
    mat_io.cpp
    mat_config.cpp
    mat_util.cpp)

add_library(mat ${MAT_SOURCES})

target_include_directories(mat PRIVATE ${gotcha_INCLUDE_DIRS})
target_link_libraries(mat PRIVATE gotcha)

set(MATIR_SOURCES
    mat_ir_pass.cpp
    mat_io.cpp
    mat_config.cpp
    mat_util.cpp)

add_library(matir ${MATIR_SOURCES})

target_include_directories(matir PRIVATE ${LLVM_INCLUDE_DIRS})
target_link_libraries(matir PRIVATE LLVMPasses)

set(MAT_RD_SOURCES
    mat_reuse_distance_main.cpp 
    mat_reuse_distance.cpp 
    mat_io.cpp 
    mat_util.cpp)
set(MAT_B2T_SOURCES
    mat_b2t_main.cpp 
    mat_reuse_distance.cpp 
    mat_io.cpp 
    mat_util.cpp)

add_executable(mat-rd ${MAT_RD_SOURCES})
add_executable(mat-b2t ${MAT_B2T_SOURCES})
