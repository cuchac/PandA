#ifndef __STRATEGY_H__
#define __STRATEGY_H__
#include "main.h"

typedef int (*strategy_function)(instance &sInstance);

int strategy_BrutalForce(instance &sInstance);

int strategy_Greedy(instance &sInstance);

int strategy_BandB(instance &sInstance);

int strategy_Dynamic(instance &sInstance);

int strategy_Genetic(instance &m_sInstance);

#endif /* __STRATEGY_H__ */