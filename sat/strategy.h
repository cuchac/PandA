#ifndef __STRATEGY_H__
#define __STRATEGY_H__


#include "main.h"

typedef int (*strategy_function)(Instance &sInstance);

int strategy_BrutalForce(Instance &sInstance);
int strategy_BandB(Instance &sInstance);
int strategy_Genetic(Instance &m_sInstance);
int strategy_Genetic_optimize(Instance &m_sInstance);

#endif /* __STRATEGY_H__ */
