cmake_minimum_required (VERSION 2.6)
project (distributed-calculating)
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
target_include_directories(lab1 PUBLIC lab1)

add_executable(lab2
        lab2/pa23.c
        lab2/pa2345.h
        lab2/banking.h
        lab2/bank_robbery.c
        lab2/logging.c
        lab2/logging.h
        lab2/ipc.c
        lab2/ipc.h
        lab2/common.h
        lab2/comm.c
        lab2/comm.h
        lab2/connect.c
        lab2/connect.h
        lab2/pa1.h
)

target_include_directories(lab2 PUBLIC lab2)
target_link_libraries(lab2 "${PROJECT_SOURCE_DIR}/lib64/libruntime.so")
