#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <sys/types.h>
#include "main.h"

bool loadData(std::ifstream &stream)
{
   while(true)
   {
      uint uiId, uiCapacity, uiCount;
      stream >> uiId;
      stream >> uiCount;
      stream >> uiCapacity;

      if(stream.eof())
         break;

      if(g_mInstances.find(uiId) != g_mInstances.end())
      {
         std::cout << "Warning: Instance " << uiId << " allready exists!" << std::endl;
      }

      instance * i = &g_mInstances[uiId];

      i->index = uiId;
      i->capacity = uiCapacity;
      i->items.resize(uiCount);

      for(uint uiIndex = 0; uiIndex < uiCount; uiIndex++)
      {
         i->items[uiIndex].index = uiIndex;
         stream >> i->items[uiIndex].weight;
         stream >> i->items[uiIndex].price;
      }
   }

   return true;
}

bool loadSolutions(std::ifstream &stream)
{
   while(true)
   {
      uint uiId, uiPrice, uiCount;
      stream >> uiId;
      stream >> uiCount;
      stream >> uiPrice;

      if(stream.eof())
         break;

      if(g_mSolutions.find(uiId) != g_mSolutions.end())
      {
         std::cout << "Warning: Solution " << uiId << " allready exists!" << std::endl;
      }

      solution * i = &g_mSolutions[uiId];

      i->index = uiId;
      i->price = uiPrice;
      i->selection.resize(uiCount);

      for(uint uiIndex = 0; uiIndex < uiCount; uiIndex++)
      {
         uint uiSelected;
         stream >> uiSelected;
         i->selection[uiIndex] = uiSelected != 0;
      }
   }
   return true;
}

void print_solution(solution &sSolution, uint uiIndex)
{
   std::cout << uiIndex << " - " << sSolution.price << " : ";
   for(selection_t::iterator it = sSolution.selection.begin(); it != sSolution.selection.end(); it++)
   {
      std::cout << ((*it)?"1" : "0") << ", ";
   }
   std::cout << std::endl;
}

double compare_solution(solution &sSolution, uint uiIndex, solution &sCorrrectSolution)
{
   if(g_uiFlags & FLAG_CHECK)
   {
      std::cout << uiIndex << " - " << sSolution.price << " : "
         << ((sCorrrectSolution.price == sSolution.price)?"correct":"wrong")
         << ":" << sCorrrectSolution.price
         << ", err:" <<  (1.0 - ((double)sSolution.price / sCorrrectSolution.price)) << std::endl;

      selection_t::iterator it2 = sCorrrectSolution.selection.begin();
      for(selection_t::iterator it = sSolution.selection.begin(); it != sSolution.selection.end(); it++, it2++)
      {
         std::cout << ((*it)?"1" : "0") << ((*it != *it2)?"*":"") << ", ";
      }

      std::cout << std::endl;
   }

   return (1.0 - ((double)sSolution.price / sCorrrectSolution.price));
}

double check_instance(instance sInstance, solution &sCorrrectSolution)
{
   return compare_solution(sInstance.result, sInstance.index, sCorrrectSolution);
}