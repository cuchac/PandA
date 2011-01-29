PROJECT(uloha_1)

cmake_minimum_required(VERSION 2.6)

link_libraries(rt)

ADD_EXECUTABLE(uloha_1 part_5.cpp part_3.cpp part_1.cpp main.cpp util.cpp)