cmake_minimum_required(VERSION 3.17)
project(MPI_EXAMPLES)

set(CMAKE_CXX_STANDARD 14)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(MPI_EXAMPLES main_comparison.cpp)
target_link_libraries (MPI_EXAMPLES ${CMAKE_THREAD_LIBS_INIT})