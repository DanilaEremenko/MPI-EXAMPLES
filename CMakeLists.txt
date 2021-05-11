cmake_minimum_required(VERSION 3.17)
project(MPI_EXAMPLES)

set(CMAKE_CXX_STANDARD 14)
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)
find_package(MPI REQUIRED)
if (MPI_FOUND)
    include_directories(SYSTEM ${MPI_INCLUDE_PATH})
else (MPI_FOUND)
    message(SEND_ERROR "This application cannot compile without MPI")
endif (MPI_FOUND)

include_directories(SYSTEM ${MPI_INCLUDE_PATH})

add_executable(main_sequential main_sequential.cpp)
add_executable(main_posix main_posix.cpp)
add_executable(main_mpi main_mpi.cpp)

target_link_libraries(main_sequential ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(main_posix ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(main_mpi ${CMAKE_THREAD_LIBS_INIT} ${MPI_LIBRARIES})