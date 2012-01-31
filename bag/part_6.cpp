#include <stdlib.h>
#include <algorithm>

#include "main.h"
#include "util.h"

uint        g_uiGenerationsSAT = 200;
uint        g_uiPopulationSAT = 100;
uint        g_uiSelectionPressureSAT = 16; // How many to "kick out" from population
uint        g_uiMutationsSAT = 72; // How many 'bits' get mutated
uint        g_uiOverloadPenaltySAT = 50; // [%]

class SolutionSAT
{
public:
	static bool sorterByHealth(const SolutionSAT *i1, const SolutionSAT *i2) {g_uiStates++; return i1->m_iHealth > i2->m_iHealth;};
	static bool sorterByPrice(const SolutionSAT *i1, const SolutionSAT *i2) {g_uiStates++; return i1->m_iPrice > i2->m_iPrice;};

	void calcHealth(instance& sInstance);
	void swap(item &item) {};
	bool isAllowed(){};

public:
	int	m_iPrice;
	bool 	m_bValid;
	int	m_iHealth;
};

class PopulationSAT
{
public:
	PopulationSAT(instance &sInstance, int iSize);

public:
	void populate();
	void select();
	void combine();
	void mutate();
	void combineOne(SolutionSAT& sParent1, SolutionSAT& sParent2, SolutionSAT& sChild);
	void mutateOne(SolutionSAT& sIndividuum, int iRandom);
	bool isFinished();
	void recalcHealth();

public:
	instance &m_sInstance;

   int m_iSize;
   std::vector<SolutionSAT> m_aGeneration;
   std::vector<SolutionSAT*> m_aGenerationSort;
   uint m_iGeneration;
};

int strategy_GeneticSAT(instance &sInstance)
{
   PopulationSAT sPopulation(sInstance, g_uiPopulationSAT);

   sPopulation.m_aGenerationSort.resize(g_uiPopulationSAT);
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

   std::sort(sPopulation.m_aGenerationSort.begin(), sPopulation.m_aGenerationSort.end(), SolutionSAT::sorterByPrice);

   for(int iIndex = 0; iIndex < sPopulation.m_iSize; iIndex++)
      if(sPopulation.m_aGenerationSort[iIndex]->isAllowed())
      {
         g_uiStates++;
         //TODO sInstance.result = *sPopulation.m_aGenerationSort[iIndex];
         break;
      }

   return 0;
}

PopulationSAT::PopulationSAT ( instance& m_sInstance, int iSize ):m_sInstance(m_sInstance),m_iSize(iSize),m_iGeneration(0)
{
   m_aGeneration.resize(iSize);
}

void PopulationSAT::populate()
{
   for(int iIndex = 0; iIndex < m_iSize; iIndex++)
   {
      g_uiStates++;
      //TODO m_aGeneration[iIndex].selection.resize(m_sInstance.items.size(), 0);
      m_aGeneration[iIndex].m_bValid = true;
   }

   for(int iIndex = 0; iIndex < (int)(m_iSize*m_sInstance.items.size()/2); iIndex++)
   {
      g_uiStates++;
      int iRandom = rand();
      m_aGeneration[iRandom%m_iSize].swap(m_sInstance.items[iRandom%m_sInstance.items.size()]);
   }
}

void PopulationSAT::select()
{
   int iSurvivorsCount = m_iSize - g_uiSelectionPressureSAT;
   std::vector<SolutionSAT> vSurvivors(iSurvivorsCount);

   std::sort(m_aGenerationSort.begin(), m_aGenerationSort.end(), SolutionSAT::sorterByHealth);

   //std::cout << "<td>" << m_aGenerationSort[0]->m_iPrice << "</td>";

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

void PopulationSAT::combine()
{
   int iSurvivorsCount = m_iSize - g_uiSelectionPressureSAT;

   for(int iIndex = iSurvivorsCount; iIndex < m_iSize; iIndex++)
   {
      g_uiStates++;
      int iFirstParent = rand() % iSurvivorsCount;
      int iSecondParent = rand() % iSurvivorsCount;

      combineOne(m_aGeneration[iFirstParent], m_aGeneration[iSecondParent], m_aGeneration[iIndex]);
   }
}

void PopulationSAT::mutate()
{
   for(int iIndex = 0; iIndex < (int)g_uiMutationsSAT; iIndex++)
   {
      g_uiStates++;
      int iRandom = rand();
      mutateOne(m_aGeneration[iRandom%m_iSize], iRandom);
   }
}

void PopulationSAT::combineOne ( SolutionSAT& sFirstParent, SolutionSAT& sSecondParent, SolutionSAT& sChild )
{
   int iRandom = rand() % m_sInstance.items.size();

   // Walk through all the gens and combine both parents
   /*TODO for(int iIndex = 0; iIndex < (int)m_sInstance.items.size(); iIndex++)
   {
      g_uiStates++;
      if(iIndex < iRandom)
         sChild.set(m_sInstance.items[iIndex], sFirstParent.selection[iIndex]);
      else
         sChild.set(m_sInstance.items[iIndex], sSecondParent.selection[iIndex]);
   }*/
}

void PopulationSAT::mutateOne ( SolutionSAT& sIndividuum, int iRandom )
{
   sIndividuum.swap(m_sInstance.items[iRandom%m_sInstance.items.size()]);
}

bool PopulationSAT::isFinished()
{
   m_iGeneration++;
   return m_iGeneration > g_uiGenerationsSAT;
}

void PopulationSAT::recalcHealth()
{
   for(int iIndex = 0; iIndex < m_iSize; iIndex++)
   {
      g_uiStates++;
      m_aGeneration[iIndex].calcHealth(m_sInstance);
   }
}

void SolutionSAT::calcHealth( instance& sInstance)
{
   /*TODO m_iHealth = m_iPrice * 1000;

   if(weight > sInstance.capacity)
      m_iHealth *= (sInstance.capacity / weight) * (1 - (g_uiOverloadPenaltySAT / 100));
	*/
}

int strategy_GeneticSAT_optimize(instance &sInstance)
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
   for(g_uiGenerationsSAT = 200; g_uiGenerationsSAT <= 200; g_uiGenerationsSAT *= 2)
   {
      for(g_uiPopulationSAT = 10; g_uiPopulationSAT <= 100; g_uiPopulationSAT += 10)
      {
         for(g_uiSelectionPressureSAT = 0; g_uiSelectionPressureSAT < g_uiPopulationSAT; g_uiSelectionPressureSAT += (g_uiPopulationSAT / 8))
         {
            for(g_uiMutationsSAT = 0; g_uiMutationsSAT < g_uiPopulationSAT; g_uiMutationsSAT += (g_uiPopulationSAT / 8))
            {
               for(g_uiOverloadPenaltySAT = 0; g_uiOverloadPenaltySAT < 100; g_uiOverloadPenaltySAT +=10)
               {
                  uiAvgPrice = 0;
                  for(int i = 0; i < 10; i++)
                  {
                     strategy_GeneticSAT(sInstance);
                     //TODO uiAvgPrice += sInstance.result.m_iPrice;
                  }
                  uiAvgPrice /= 10;

                  if(uiAvgPrice > uiBestAvgPrice)
                  {
                     best = sInstance.result;
                     uiBestAvgPrice = uiAvgPrice;
                     uiBestGenerations = g_uiGenerationsSAT;
                     uiBestPopulation = g_uiPopulationSAT;
                     uiBestSelectionPressure = g_uiSelectionPressureSAT;
                     uiBestMutations = g_uiMutationsSAT;
                     uiBestOverloadPenalty = g_uiOverloadPenaltySAT;
                  }
               }
            }
         }
      }
   }

   sInstance.result = best;
   std::cout << "Best result for " << sInstance.index << ":" << std::endl <<
                "average m_iPrice: " << uiBestAvgPrice << std:: endl << "correct m_iPrice: " << g_mSolutions[sInstance.index].price << std::endl <<
                "generations: " << uiBestGenerations << std::endl << "PopulationSAT: " << uiBestPopulation << std::endl << "selection pressure: " << uiBestSelectionPressure << std::endl << "mutations: " << uiBestMutations << std::endl << "overload penalty: " << uiBestOverloadPenalty << std::endl;

   return 0;
}
