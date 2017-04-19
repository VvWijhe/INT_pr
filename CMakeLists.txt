cmake_minimum_required(VERSION 3.7)
project(INT_pr)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        screen/screen.c
        screen/screen.h)

add_executable(INT_pr ${SOURCE_FILES})