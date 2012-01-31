PROJECT(bag)

cmake_minimum_required(VERSION 2.6)

link_libraries(rt)

ADD_EXECUTABLE(bag part_6.cpp part_5.cpp part_3.cpp part_1.cpp main.cpp util.cpp)