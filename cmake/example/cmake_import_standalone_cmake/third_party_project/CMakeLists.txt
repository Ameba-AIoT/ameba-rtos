cmake_minimum_required(VERSION 3.10)

project(foo_cmake C)

add_library(foo_cmake foo_cmake.c)

target_include_directories(foo_cmake PUBLIC include)