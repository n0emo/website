cmake_minimum_required(VERSION 3.1...3.10)

project(mew)

add_library(mew STATIC ./src/all.c)

target_include_directories(mew PUBLIC ./include/)

