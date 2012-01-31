PROJECT(sat)

cmake_minimum_required(VERSION 2.6)

link_libraries(rt)

ADD_EXECUTABLE(sat part_6.cpp main.cpp util.cpp)
