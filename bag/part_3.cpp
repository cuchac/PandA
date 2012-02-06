#include <stdlib.h>
#include <algorithm>
#include <math.h>
#include "main.h"
#include "util.h"
#include <hash_map>

int strategy_BandB_recurse(instance &sInstance, solution &sCurrentSolution, uint uiLevel);

int strategy_BandB(instance &sInstance)
{
   solution sCurrentSolution;
   sCurrentSolution.selection.resize(sInstance.items.size(), 0);

   strategy_BandB_recurse(sInstance, sCurrentSolution, 0);

   return 0;
}

int strategy_BandB_recurse(instance &sInstance, solution &sCurrentSolution, uint uiLevel)
{
   g_uiStates++;
   if(sInstance.result.price < sCurrentSolution.price)
      sInstance.result = sCurrentSolution;

   int uiRestPrice = 0;
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

int g_iDynamicScaledownBits = 0;

class hashMap: public __gnu_cxx::hash_map<int, int>
{
public:
   int& operator[](const key_type& __key){
      if(g_iDynamicScaledownBits)
         return __gnu_cxx::hash_map<int, int>::operator[](__key >> g_iDynamicScaledownBits);
      else
         return __gnu_cxx::hash_map<int, int>::operator[](__key);
   }

   iterator find(const key_type& __key){
      if(g_iDynamicScaledownBits)
         return __gnu_cxx::hash_map<int, int>::find(__key >> g_iDynamicScaledownBits);
      else
         return __gnu_cxx::hash_map<int, int>::find(__key);
   }

   const_iterator find(const key_type& __key) const{
      if(g_iDynamicScaledownBits)
         return __gnu_cxx::hash_map<int, int>::find(__key >> g_iDynamicScaledownBits);
      else
         return __gnu_cxx::hash_map<int, int>::find(__key);
   }
   
};


int strategy_Dynamic_recurse(instance &sInstance, solution &sCurrentSolution, int iLevel, int iCapacity);
std::vector<hashMap> g_sCache;

int strategy_Dynamic(instance &sInstance)
{
   sInstance.result.selection.resize(sInstance.items.size(), 0);

   g_sCache.clear();
   g_sCache.resize(sInstance.items.size());

   strategy_Dynamic_recurse(sInstance, sInstance.result, sInstance.items.size()-1, sInstance.capacity);

   int iCapacity = sInstance.capacity;
   for(int iLevel = sInstance.items.size()-1; iLevel > 0; iLevel--)
      if(g_sCache[iLevel][iCapacity] == g_sCache[iLevel-1][iCapacity])
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
   g_uiStates++;
   item &sItem = sInstance.items[iLevel];

   if(iCapacity <= 0 || iLevel < 0)
      return 0;

   if(g_sCache[iLevel].find(iCapacity) != g_sCache[iLevel].end())
      return g_sCache[iLevel][iCapacity];

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
      g_sCache[iLevel][iCapacity] = uiPriceLeave;
      return uiPriceLeave;
   }
   else
   {
      g_sCache[iLevel][iCapacity] = uiPricePut;
      return uiPricePut;
   }
}

int strategy_GreedyBest(instance &sInstance)
{
   sInstance.result.selection.resize(sInstance.items.size(), 0);

   std::sort(sInstance.items.begin(), sInstance.items.end(), item::sorter_by_quality);

   for(items_t::iterator it = sInstance.items.begin(); it != sInstance.items.end() && (sInstance.result.weight + it->weight <= sInstance.capacity); it++)
   {
      g_uiStates++;
      sInstance.result.add(*it);
   }

   std::sort(sInstance.items.begin(), sInstance.items.end(), item::sorter_by_price);

   if(sInstance.items[0].price > sInstance.result.price)
   {
      for(items_t::iterator it = sInstance.items.begin(); it != sInstance.items.end(); it++)
      {
	 g_uiStates++;
         sInstance.result.del(*it);
      }
      sInstance.result.add(sInstance.items[0]);
   }

   return 0;
}

int strategy_FPTAS(instance &sInstance)
{
   g_iDynamicScaledownBits = 1;
   
   if(getenv("FPTAS_SCALEDOWN"))
      g_iDynamicScaledownBits = atoi(getenv("FPTAS_SCALEDOWN"));

   std::cout << "Scaling: " << g_iDynamicScaledownBits << std::endl;

   return strategy_Dynamic(sInstance);
}