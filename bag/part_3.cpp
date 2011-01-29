#include <stdlib.h>
#include <algorithm>
#include "main.h"
#include "util.h"

int strategy_BandB_recurse(instance &sInstance, solution &sCurrentSolution, uint uiLevel);

int strategy_BandB(instance &sInstance)
{
   solution sCurrentSolution;
   sCurrentSolution.selection.resize(sInstance.items.size(), 0);

   std::sort(sInstance.items.begin(), sInstance.items.end(), item::sorter_by_quality);

   strategy_BandB_recurse(sInstance, sCurrentSolution, 0);

   std::sort(sInstance.items.begin(), sInstance.items.end(), item::sorter_by_id);

   if(sCurrentSolution.price != 0)
      std::cout << "Algorythm error! sCurrentSolution.price:" << sCurrentSolution.price;

   if(sCurrentSolution.weight != 0)
      std::cout << "Algorythm error! sCurrentSolution.weight:" << sCurrentSolution.weight;

   return 0;
}

int strategy_BandB_recurse(instance &sInstance, solution &sCurrentSolution, uint uiLevel)
{
   if(sInstance.result.price < sCurrentSolution.price)
      sInstance.result = sCurrentSolution;

   uint uiRestPrice = 0;
   for(items_t::iterator it = sInstance.items.begin()+uiLevel; it != sInstance.items.end(); it++)
      uiRestPrice += it->price;
   if(uiRestPrice + sCurrentSolution.price < sInstance.result.price)
      return 0;

   if(uiLevel < sInstance.items.size())
   {
      item sItem = sInstance.items[uiLevel];
      if(sCurrentSolution.weight + sItem.weight <= sInstance.capacity)
      {
         sCurrentSolution.add(sItem);

         strategy_BandB_recurse(sInstance, sCurrentSolution, uiLevel+1);

         sCurrentSolution.del(sItem);
      }

      strategy_BandB_recurse(sInstance, sCurrentSolution, uiLevel+1);
   }

   return 0;
}

int strategy_Dynamic_recurse(instance &sInstance, solution &sCurrentSolution, int iLevel, int iCapacity);

typedef struct cacheItem_t {
	int 		m_uiPrice;
	//solution m_sSolution;
} cacheItem;

std::vector<std::map<int, cacheItem> > g_sCache;

int strategy_Dynamic(instance &sInstance)
{
   /*solution sCurrentSolution;
   sCurrentSolution.selection.resize(sInstance.items.size(), 0);*/
   sInstance.result.selection.resize(sInstance.items.size(), 0);

	g_sCache.clear();
	g_sCache.resize(sInstance.items.size());
	
   strategy_Dynamic_recurse(sInstance, sInstance.result, sInstance.items.size()-1, sInstance.capacity);

	int iCapacity = sInstance.capacity;
	for(int iLevel = sInstance.items.size()-1; iLevel > 0; iLevel --)
		if(g_sCache[iLevel][iCapacity].m_uiPrice == g_sCache[iLevel-1][iCapacity].m_uiPrice)
		{
			sInstance.result.del(sInstance.items[iLevel]);
		}
		else
		{
			sInstance.result.add(sInstance.items[iLevel]);
			iCapacity -= sInstance.items[iLevel].weight;
		}
	if(iCapacity >= sInstance.items[0].weight)
	{
		sInstance.result.add(sInstance.items[0]);
		iCapacity -= sInstance.items[0].weight;
	}
	else
	{
		sInstance.result.del(sInstance.items[0]);
	}

   return 0;
}

int strategy_Dynamic_recurse(instance &sInstance, solution &sCurrentSolution, int iLevel, int iCapacity)
{
   item &sItem = sInstance.items[iLevel];

   if(iCapacity <= 0 || iLevel < 0)
      return 0;

	if(g_sCache[iLevel].find(iCapacity) != g_sCache[iLevel].end())
		return g_sCache[iLevel][iCapacity].m_uiPrice;


   int uiPriceLeave = 0, uiPricePut = 0;

   sCurrentSolution.del(sItem);
   uiPriceLeave = strategy_Dynamic_recurse(sInstance, sCurrentSolution, iLevel-1, iCapacity);

	if((iCapacity-sInstance.items[iLevel].weight) >= 0)
	{
		sCurrentSolution.add(sItem);
		uiPricePut = strategy_Dynamic_recurse(sInstance, sCurrentSolution, iLevel-1, iCapacity-sInstance.items[iLevel].weight);
		uiPricePut += sInstance.items[iLevel].price;
	}

   if(uiPriceLeave >= uiPricePut)
	{
		g_sCache[iLevel][iCapacity].m_uiPrice = uiPriceLeave;

		return uiPriceLeave;
	}
	else
	{
		g_sCache[iLevel][iCapacity].m_uiPrice = uiPricePut;
		
		return uiPricePut;
	}
}