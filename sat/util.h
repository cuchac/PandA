#ifndef __UTIL_H__
#define __UTIL_H__

#include "main.h"
#include <iostream>
#include <sys/types.h>

bool loadData(std::ifstream &stream);
bool loadSolutions(std::ifstream &stream);
void print_solution(Solution &sSolution, uint uiIndex);
double compare_solution(Solution &sSolution, uint uiIndex, Solution &sCorrrectSolution);
double check_instance(Instance sInstance, Solution &sCorrrectSolution);

#endif /* __UTIL_H__ */
