cmake_minimum_required(VERSION 3.1)
project (mat-llvm CXX)

option(BUILD_SHARED_LIBS "Build shared libs" TRUE)

find_package(gotcha 1.0 REQUIRED)
find_package(LLVM REQUIRED)

add_subdirectory(src)
