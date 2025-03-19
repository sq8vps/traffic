cmake_minimum_required(VERSION 3.10.0)
project(ls_rs_hlfs VERSION 0.1.0 LANGUAGES C)

add_executable(ls_rs_hlfs main.c)

target_link_libraries(ls_rs_hlfs PRIVATE SimLib)