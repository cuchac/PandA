#include <iostream>
#include <fstream>
#include <limits>
#include <stdlib.h>
#include <sys/types.h>
#include "main.h"

int g_iInstanceIndex = 0;

bool loadData(std::ifstream &stream)
{
   while(true)
   {
      char c = 0;
      stream >> c;

      if(!stream.good())
         break;

      if (c != 'p')
      {
         stream.ignore(std::numeric_limits<int>::max(), '\n');
         continue;
      }

      uint uiId, uiClauses, uiVariables;
      uiId = g_iInstanceIndex++;
      std::string sType;
      stream >> sType;
      stream >> uiVariables;
      stream >> uiClauses;

      Instance * i = &g_mInstances[uiId];

      i->m_iIndex = uiId;
      i->m_vVariables.resize(uiVariables);
      i->m_vClauses.resize(uiClauses);
      i->m_iVariables = uiVariables;
      i->m_iClauses = uiClauses;

      // Try load weights
      stream.ignore(1, '\n');
      stream >> c;
      if (c == 'w')
      {
         // We have weights, load them
         for(uint uiVariable = 0; uiVariable < uiVariables; uiVariable++)
         {
            i->m_vVariables[uiVariable].m_iIindex = uiVariable;
            stream >> i->m_vVariables[uiVariable].m_iWeight;
         }
      }
      else
      {
         // We DONT have weights, so generate them
         for(uint uiVariable = 0; uiVariable < uiVariables; uiVariable++)
         {
            i->m_vVariables[uiVariable].m_iIindex = uiVariable;
            i->m_vVariables[uiVariable].m_iWeight = rand() % 100;
         }

         // Return character
         stream.putback(c);
      }

      for(uint uiClause = 0; uiClause < uiClauses; uiClause++)
      {
         uint uiVariable = 0;
         int iValue = 0;
         while(true)
         {
            stream >> iValue;
            if(iValue != 0)
               i->m_vClauses[uiClause].m_vVariables[uiVariable++] = iValue;
            else
               break;
         }
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

      Solution * i = &g_mSolutions[uiId];

      //i->m_iIndex = uiId;
      i->m_iPrice = uiPrice;
      i->m_vSelection.resize(uiCount);

      for(uint uiIndex = 0; uiIndex < uiCount; uiIndex++)
      {
         uint uiSelected;
         stream >> uiSelected;
         i->m_vSelection[uiIndex] = uiSelected != 0;
      }
   }

   return true;
}

void print_solution(Solution &sSolution, uint uiIndex)
{
   std::cout << "m_iIndex: " << uiIndex << ", m_price: " << sSolution.m_iPrice << " - ";
   for(selection_t::iterator it = sSolution.m_vSelection.begin(); it != sSolution.m_vSelection.end(); it++)
   {
      std::cout << ((*it)?"1" : "0") << ", ";
   }

   std::cout << std::endl;
}

double compare_solution(Solution &sSolution, uint uiIndex, Solution &sCorrrectSolution)
{
   if(g_uiFlags & FLAG_CHECK)
   {
      std::cout << "m_iIndex: " << uiIndex << ", m_iPrice: " << sSolution.m_iPrice << " -> "
         << ((sCorrrectSolution.m_iPrice == sSolution.m_iPrice)?"correct":"wrong")
         << ", " << "correct m_iPrice: " << sCorrrectSolution.m_iPrice
         << ", rel. err: " <<  (1.0 - ((double)sSolution.m_iPrice / sCorrrectSolution.m_iPrice)) << std::endl;

      /*selection_t::iterator it2 = sCorrrectSolution.m_vSelection.begin();
      for(selection_t::iterator it = sSolution.m_vSelection.begin(); it != sSolution.m_vSelection.end(); it++, it2++)
      {
         std::cout << ((*it)?"1" : "0") << ((*it != *it2)?"*":"") << ", ";
      }

      std::cout << std::endl;*/
   }

   return (1.0 - ((double)sSolution.m_iPrice / sCorrrectSolution.m_iPrice));
}

double check_instance(Instance sInstance, Solution &sCorrrectSolution)
{
   return compare_solution(sInstance.m_sResult, sInstance.m_iIndex, sCorrrectSolution);
}
