cmake_minimum_required(VERSION 3.0)

project(lock2del)

add_executable(lock2del lock2del.c)
target_compile_options(lock2del PRIVATE -Wall -Wextra)

install(TARGETS lock2del RUNTIME DESTINATION bin)
