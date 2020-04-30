cmake_minimum_required (VERSION 2.6)
project (distributed-calculating)
include_directories(lab1)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -Werror -pedantic")

add_executable(lab1
        lab1/main.c
        lab1/logging.c
        lab1/logging.h
        lab1/ipc.c
        lab1/ipc.h
        lab1/common.h
        lab1/comm.c
        lab1/comm.h
        lab1/connect.c
        lab1/connect.h
        lab1/pa1.h
)
