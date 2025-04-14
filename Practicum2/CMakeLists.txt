cmake_minimum_required(VERSION 3.27)
project(Practicum2 C)

set(CMAKE_C_STANDARD 11)

add_executable(Practicum2 main.c
        messenger.h
        messenger.c
        client.c)
