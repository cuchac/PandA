#include <stdlib.h>
#include <algorithm>
#include "main.h"
#include "util.h"

int strategy_BrutalForce_recurse(instance &sInstance, solution &sCurrentSolution, uint uiLevel);

int strategy_BrutalForce(instance &sInstance)
{
   solution sCurrentSolution;
   sCurrentSolution.selection.resize(sInstance.items.size(), 0);

   strategy_BrutalForce_recurse(sInstance, sCurrentSolution, 0);

   if(sCurrentSolution.price != 0)
      std::cout << "Algorythm error! sCurrentSolution.price:" << sCurrentSolution.price;

   if(sCurrentSolution.weight != 0)
      std::cout << "Algorythm error! sCurrentSolution.weight:" << sCurrentSolution.weight;

   return 0;
}

int strategy_BrutalForce_recurse(instance &sInstance, solution &sCurrentSolution, uint uiLevel)
{
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
      sInstance.result.add(*it);

   std::sort(sInstance.items.begin(), sInstance.items.end(), item::sorter_by_id);

   return 0;
}

/*
int strategy_brutal_force_recurse(instance &sInstance, solution &sCurrentSolution, uint uiLevel)
{
   for(uint uiIndex = 0; uiIndex < sCurrentSolution.selection.size(); uiIndex++)
   {
      item sItem = sInstance.items[uiIndex];
      if(!sCurrentSolution.selection[uiIndex] && sCurrentSolution.weight + sItem.weight <= sInstance.capacity)
      {
         sCurrentSolution.add(sItem);

         print_solution(sCurrentSolution, 0);

         if(sMaxPriceSolution.price < sCurrentSolution.price)
            sMaxPriceSolution = sCurrentSolution;

         strategy_brutal_force_recurse(sInstance, sCurrentSolution);

         sCurrentSolution.del(sItem);
      }
   }
   return 0;
}
*/