#include <stdlib.h>
#include <algorithm>

#include "main.h"
#include "util.h"

uint        g_uiGenerations = 200;
uint        g_uiPopulation = 100;
uint        g_uiSelectionPressure = 16; // How many to "kick out" from population
uint        g_uiMutations = 72; // How many 'bits' get mutated
uint        g_uiOverloadPenalty = 50; // [%]

class solution_gen:public solution
{
   public:
      static bool sorterByHealth(const solution_gen *i1, const solution_gen *i2) {g_uiStates++; return i1->m_iHealth > i2->m_iHealth;};
      static bool sorterByPrice(const solution_gen *i1, const solution_gen *i2) {g_uiStates++; return i1->price > i2->price;};

      void calcHealth(instance& sInstance);
      void set(item& item, bool bSet) {if(bSet)add(item); else del(item);};
      void swap(item &item) {if(selection[item.index])del(item); else add(item);};

   public:
      bool m_bValid;
      int m_iHealth;
};

class population
{
   public:
      population(instance &sInstance, int iSize);

   public:
      void populate();
      void select();
      void combine();
      void mutate();
      void combineOne(solution_gen& sParent1, solution_gen& sParent2, solution_gen& sChild);
      void mutateOne(solution_gen& sIndividuum, int iRandom);
      bool isFinished();
      void recalcHealth();

   public:
      instance &m_sInstance;

   int m_iSize;
   std::vector<solution_gen> m_aGeneration;
   std::vector<solution_gen*> m_aGenerationSort;
   uint m_iGeneration;
};

int strategy_Genetic(instance &sInstance)
{
   population sPopulation(sInstance, g_uiPopulation);

   sPopulation.m_aGenerationSort.resize(g_uiPopulation);
   for(int iIndex = 0; iIndex < sPopulation.m_iSize; iIndex++)
   {
      g_uiStates++;
      sPopulation.m_aGenerationSort[iIndex] = &(sPopulation.m_aGeneration[iIndex]);
   }

   sPopulation.populate();

   while(!sPopulation.isFinished())
   {
      // Update health
      sPopulation.recalcHealth();

      // Selection
      sPopulation.select();

      // Combination
      sPopulation.combine();

      // Mutation
      sPopulation.mutate();
   }

   std::sort(sPopulation.m_aGenerationSort.begin(), sPopulation.m_aGenerationSort.end(), solution_gen::sorterByPrice);

   for(int iIndex = 0; iIndex < sPopulation.m_iSize; iIndex++)
      if(sPopulation.m_aGenerationSort[iIndex]->weight <= sInstance.capacity)
      {
         g_uiStates++;
         sInstance.result = *sPopulation.m_aGenerationSort[iIndex];
         break;
      }

   return 0;
}

population::population ( instance& m_sInstance, int iSize ):m_sInstance(m_sInstance),m_iSize(iSize),m_iGeneration(0)
{
   m_aGeneration.resize(iSize);
}

void population::populate()
{
   for(int iIndex = 0; iIndex < m_iSize; iIndex++)
   {
      g_uiStates++;
      m_aGeneration[iIndex].selection.resize(m_sInstance.items.size(), 0);
      m_aGeneration[iIndex].m_bValid = true;
   }

   for(int iIndex = 0; iIndex < (int)(m_iSize*m_sInstance.items.size()/2); iIndex++)
   {
      g_uiStates++;
      int iRandom = rand();
      m_aGeneration[iRandom%m_iSize].swap(m_sInstance.items[iRandom%m_sInstance.items.size()]);
   }
}

void population::select()
{
   int iSurvivorsCount = m_iSize - g_uiSelectionPressure;
   std::vector<solution_gen> vSurvivors(iSurvivorsCount);

   std::sort(m_aGenerationSort.begin(), m_aGenerationSort.end(), solution_gen::sorterByHealth);

   //std::cout << "<td>" << m_aGenerationSort[0]->price << "</td>";

   int iRankSum = m_iSize * (m_iSize+1) / 2;

   for(int iIndex = 0; iIndex < iSurvivorsCount; iIndex++)
   {
      g_uiStates++;
      int iRandom = rand() % iRankSum;

      int iCurRankSum = 0;
      int iRank;
      for(iRank = m_iSize; iRank > 0; iRank--)
      {
         iCurRankSum+=iRank;
         if(iRandom < iCurRankSum)
         {
            vSurvivors[iIndex] = *(m_aGenerationSort[m_iSize - iRank]);
            break;
         }
      }
      if(iRank<=0)
      iCurRankSum = 0;
   }

   for(int iIndex = 0; iIndex < iSurvivorsCount; iIndex++)
   {
      g_uiStates++;
      m_aGeneration[iIndex] = vSurvivors[iIndex];
   }
}

void population::combine()
{
   int iSurvivorsCount = m_iSize - g_uiSelectionPressure;

   for(int iIndex = iSurvivorsCount; iIndex < m_iSize; iIndex++)
   {
      g_uiStates++;
      int iFirstParent = rand() % iSurvivorsCount;
      int iSecondParent = rand() % iSurvivorsCount;

      combineOne(m_aGeneration[iFirstParent], m_aGeneration[iSecondParent], m_aGeneration[iIndex]);
   }
}

void population::mutate()
{
   for(int iIndex = 0; iIndex < (int)g_uiMutations; iIndex++)
   {
      g_uiStates++;
      int iRandom = rand();
      mutateOne(m_aGeneration[iRandom%m_iSize], iRandom);
   }
}

void population::combineOne ( solution_gen& sFirstParent, solution_gen& sSecondParent, solution_gen& sChild )
{
   int iRandom = rand() % m_sInstance.items.size();

   // Walk through all the gens and combine both parents
   for(int iIndex = 0; iIndex < (int)m_sInstance.items.size(); iIndex++)
   {
      g_uiStates++;
      if(iIndex < iRandom)
         sChild.set(m_sInstance.items[iIndex], sFirstParent.selection[iIndex]);
      else
         sChild.set(m_sInstance.items[iIndex], sSecondParent.selection[iIndex]);
   }
}

void population::mutateOne ( solution_gen& sIndividuum, int iRandom )
{
   sIndividuum.swap(m_sInstance.items[iRandom%m_sInstance.items.size()]);
}

bool population::isFinished()
{
   m_iGeneration++;
   return m_iGeneration > g_uiGenerations;
}

void population::recalcHealth()
{
   for(int iIndex = 0; iIndex < m_iSize; iIndex++)
   {
      g_uiStates++;
      m_aGeneration[iIndex].calcHealth(m_sInstance);
   }
}

void solution_gen::calcHealth( instance& sInstance)
{
   m_iHealth = price * 1000;

   if(weight > sInstance.capacity)
      m_iHealth *= (sInstance.capacity / weight) * (1 - (g_uiOverloadPenalty / 100));
}

int strategy_Genetic_optimize(instance &sInstance)
{
   solution best;
   uint uiAvgPrice = 0;
   uint uiBestAvgPrice = 0;
   uint uiBestGenerations = 0;
   uint uiBestPopulation = 0;
   uint uiBestSelectionPressure = 0;
   uint uiBestMutations = 0;
   uint uiBestOverloadPenalty = 0;

   // Find best parameters
   for(g_uiGenerations = 200; g_uiGenerations <= 200; g_uiGenerations *= 2)
   {
      for(g_uiPopulation = 10; g_uiPopulation <= 100; g_uiPopulation += 10)
      {
         for(g_uiSelectionPressure = 0; g_uiSelectionPressure < g_uiPopulation; g_uiSelectionPressure += (g_uiPopulation / 8))
         {
            for(g_uiMutations = 0; g_uiMutations < g_uiPopulation; g_uiMutations += (g_uiPopulation / 8))
            {
               for(g_uiOverloadPenalty = 0; g_uiOverloadPenalty < 100; g_uiOverloadPenalty +=10)
               {
                  uiAvgPrice = 0;
                  for(int i = 0; i < 10; i++)
                  {
                     strategy_Genetic(sInstance);
                     uiAvgPrice += sInstance.result.price;
                  }
                  uiAvgPrice /= 10;

                  if(uiAvgPrice > uiBestAvgPrice)
                  {
                     best = sInstance.result;
                     uiBestAvgPrice = uiAvgPrice;
                     uiBestGenerations = g_uiGenerations;
                     uiBestPopulation = g_uiPopulation;
                     uiBestSelectionPressure = g_uiSelectionPressure;
                     uiBestMutations = g_uiMutations;
                     uiBestOverloadPenalty = g_uiOverloadPenalty;
                  }
               }
            }
         }
      }
   }

   sInstance.result = best;
   std::cout << "Best result for " << sInstance.index << ":" << std::endl <<
                "average price: " << uiBestAvgPrice << std:: endl << "correct price: " << g_mSolutions[sInstance.index].price << std::endl <<
                "generations: " << uiBestGenerations << std::endl << "population: " << uiBestPopulation << std::endl << "selection pressure: " << uiBestSelectionPressure << std::endl << "mutations: " << uiBestMutations << std::endl << "overload penalty: " << uiBestOverloadPenalty << std::endl;

   return 0;
}
