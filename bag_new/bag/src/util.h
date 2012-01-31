#ifndef __UTIL_H__
#define __UTIL_H__
#include "main.h"
#include <iostream>
#include <sys/types.h>

bool loadData(std::ifstream &stream);

bool loadSolutions(std::ifstream &stream);

void print_solution(solution &sSolution, uint uiIndex);

double compare_solution(solution &sSolution, uint uiIndex, solution &sCorrrectSolution);

double check_instance(instance sInstance, solution &sCorrrectSolution);

#endif /* __UTIL_H__ */
