cmake_minimum_required(VERSION 3.12)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
project(Glad)

add_library(glad SHARED include/glad/glad.h src/glad.c)
target_include_directories(glad PUBLIC include/)