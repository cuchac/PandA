#include <stdlib.h>
#include <algorithm>
#include <math.h>
#include "main.h"
#include "util.h"

int strategy_BrutalForce_recurse(instance &sInstance, solution &sCurrentSolution, uint uiLevel);

int strategy_BrutalForce(instance &sInstance)
{
   solution sCurrentSolution;
   sCurrentSolution.selection.resize(sInstance.items.size(), 0);

   strategy_BrutalForce_recurse(sInstance, sCurrentSolution, 0);

   return 0;
}

int strategy_BrutalForce_recurse(instance &sInstance, solution &sCurrentSolution, uint uiLevel)
{
   g_uiStates++;

   if(sCurrentSolution.weight <= sInstance.capacity && sInstance.result.price < sCurrentSolution.price)
      sInstance.result = sCurrentSolution;

   if(uiLevel < sInstance.items.size())
   {
      item sItem = sInstance.items[uiLevel];

      sCurrentSolution.add(sItem);
      strategy_BrutalForce_recurse(sInstance, sCurrentSolution, uiLevel+1);
      sCurrentSolution.del(sItem);
      strategy_BrutalForce_recurse(sInstance, sCurrentSolution, uiLevel+1);
   }

   return 0;
}

int strategy_Greedy(instance &sInstance)
{
   sInstance.result.selection.resize(sInstance.items.size(), 0);

   std::sort(sInstance.items.begin(), sInstance.items.end(), item::sorter_by_quality);

   for(items_t::iterator it = sInstance.items.begin(); it != sInstance.items.end() && (sInstance.result.weight + it->weight <= sInstance.capacity); it++)
   {
      g_uiStates++;
      sInstance.result.add(*it);
   }

   //std::sort(sInstance.items.begin(), sInstance.items.end(), item::sorter_by_id);

   return 0;
}
