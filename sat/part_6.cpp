#include <stdlib.h>
#include <algorithm>

#include "main.h"
#include "util.h"

bool g_bBandB = false;
int g_iMutations;
int g_iSelectionPressure;
int g_iGenerations = 160;
int g_iPopulation = 80;
float g_fSelectionPressure = 0.2; // How many to "kick out" from population [%]
float g_fMutations = 0.2;  // How many 'bits' get mutated [%]
float g_fNotSatisfiedPenalty = 4.0;

class Population
{
public:
   Population(Instance &sInstance, int iSize);

public:
   void populate();
   void select();
   void combine();
   void mutate();
   void combineOne(Solution& sParent1, Solution& sParent2, Solution& sChild);
   void mutateOne(Solution& sIndividuum, int iRandom);
   bool isFinished();
   void recalcHealth();
   void updateSatisfied();

public:
   Instance &m_sInstance;

   int m_iSize;
   std::vector<Solution> m_aGeneration;
   std::vector<Solution*> m_aGenerationSort;
   uint m_iGeneration;
};

int strategy_BrutalForce(Instance &sInstance)
{
   Solution sSolution(sInstance.m_iVariables);
   int iLastIndex = 0;
   int iSatisfiable = 0;

   for(long long iIndex = 0; iIndex < 1 << sInstance.m_iVariables; iIndex++)
   {
      for(int iVarIndex = 0; iVarIndex < sInstance.m_iVariables; iVarIndex++)
      {
         if((iLastIndex & (1 << iVarIndex)) != (iIndex & (1 << iVarIndex)))
         {
            sSolution.set(sInstance.m_vVariables[iVarIndex], (iIndex & (1 << iVarIndex)) > 0);
         }
      }
      sSolution.updateSatisfied(&sInstance);
      iLastIndex = iIndex;
      g_uiStates++;
      
      if(sSolution.m_bSattisfied) iSatisfiable++;
      if(sSolution.m_bSattisfied && (sSolution.m_iPrice > sInstance.m_sResult.m_iPrice))
         sInstance.m_sResult = sSolution;
   }
   std::cout << "Satisfiable: " << iSatisfiable << std::endl;

   return 0;
}

int strategy_BandB(Instance &sInstance)
{
   g_bBandB = true;
   strategy_BrutalForce(sInstance);
   g_bBandB = false;

   return 0;
}

int strategy_Genetic(Instance &sInstance)
{
   g_iMutations = sInstance.m_iVariables * g_iPopulation * g_fMutations;
   g_iSelectionPressure = g_iPopulation * g_fSelectionPressure;
   Population sPopulation(sInstance, g_iPopulation);

   sPopulation.m_aGenerationSort.resize(g_iPopulation);
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

      // Mutation
      sPopulation.updateSatisfied();
   }

   std::sort(sPopulation.m_aGenerationSort.begin(), sPopulation.m_aGenerationSort.end(), Solution::sorterByPrice);

   for(int iIndex = 0; iIndex < sPopulation.m_iSize; iIndex++)
      if(sPopulation.m_aGenerationSort[iIndex]->m_bSattisfied)
      {
         g_uiStates++;
         sInstance.m_sResult = *sPopulation.m_aGenerationSort[iIndex];
         break;
      }

   return 0;
}

Population::Population(Instance& m_sInstance, int iSize): m_sInstance(m_sInstance), m_iSize(iSize), m_iGeneration(0)
{
   m_aGeneration.resize(iSize);
}

void Population::populate()
{
   for(int iIndex = 0; iIndex < m_iSize; iIndex++)
   {
      g_uiStates++;
      m_aGeneration[iIndex].m_vSelection.resize(m_sInstance.m_iVariables, 0);
   }

   for(int iIndex = 0; iIndex < (int)(m_iSize*m_sInstance.m_vClauses.size()*10); iIndex++)
   {
      g_uiStates++;
      int iRandom = rand();
      m_aGeneration[iRandom%m_iSize].swap(m_sInstance.m_vVariables[iRandom%m_sInstance.m_iVariables]);
   }

   for(int iIndex = 0; iIndex < m_iSize; iIndex++)
   {
      m_aGeneration[iIndex].updateSatisfied(&m_sInstance);
   }
}

void Population::select()
{
   int iSurvivorsCount = m_iSize - g_iSelectionPressure;
   std::vector<Solution> vSurvivors(iSurvivorsCount);

   std::sort(m_aGenerationSort.begin(), m_aGenerationSort.end(), Solution::sorterByHealth);

   //std::cout << m_aGenerationSort[0]->m_iPrice << " ";
   //std::cout << "<td>" << m_aGenerationSort[0]->m_iPrice << "</td>";

   //int iRankSum = m_iSize * (m_iSize+1) / 2;
   int iRankSum = 0;
   for(int iIndex = 0; iIndex < m_iSize; iIndex++)
   {
      iRankSum += m_aGenerationSort[iIndex]->m_iHealth;
   }

   for(int iIndex = 0; iIndex < iSurvivorsCount; iIndex++)
   {
      g_uiStates++;
      int iRandom = rand() % iRankSum;

      int iCurRankSum = 0;
      for(int iRank = 0; iRank < m_iSize; iRank++)
      {
         iCurRankSum += m_aGenerationSort[iRank]->m_iHealth;
         if(iCurRankSum > iRandom)
         {
            vSurvivors[iIndex] = *(m_aGenerationSort[iRank]);
            break;
         }
      }
   }

   for(int iIndex = 0; iIndex < iSurvivorsCount; iIndex++)
   {
      g_uiStates++;
      m_aGeneration[iIndex] = vSurvivors[iIndex];
   }
}

void Population::combine()
{
   int iSurvivorsCount = m_iSize - g_iSelectionPressure;

   for(int iIndex = iSurvivorsCount; iIndex < m_iSize; iIndex++)
   {
      g_uiStates++;
      int iFirstParent = rand() % iSurvivorsCount;
      int iSecondParent = rand() % iSurvivorsCount;

      combineOne(m_aGeneration[iFirstParent], m_aGeneration[iSecondParent], m_aGeneration[iIndex]);
   }
}

void Population::mutate()
{
   for(int iIndex = 0; iIndex < (int)g_iMutations; iIndex++)
   {
      g_uiStates++;
      int iRandom = rand();
      mutateOne(m_aGeneration[iRandom%m_iSize], iRandom);
   }
}

void Population::combineOne(Solution& sFirstParent, Solution& sSecondParent, Solution& sChild)
{
   int iRandom = rand() % m_sInstance.m_iVariables;

   // Walk through all the gens and combine both parents
   for(int iIndex = 0; iIndex < (int)m_sInstance.m_iVariables; iIndex++)
   {
      g_uiStates++;
      if(iIndex < iRandom)
         sChild.set(m_sInstance.m_vVariables[iIndex], sFirstParent.m_vSelection[iIndex]);
      else
         sChild.set(m_sInstance.m_vVariables[iIndex], sSecondParent.m_vSelection[iIndex]);
   }
}

void Population::mutateOne(Solution& sIndividuum, int iRandom)
{
   sIndividuum.swap(m_sInstance.m_vVariables[iRandom%m_sInstance.m_vVariables.size()]);
}

bool Population::isFinished()
{
   m_iGeneration++;
   return m_iGeneration > g_iGenerations;
}

void Population::recalcHealth()
{
   for(int iIndex = 0; iIndex < m_iSize; iIndex++)
   {
      g_uiStates++;
      m_aGeneration[iIndex].calcHealth(m_sInstance);
   }
}

void Solution::calcHealth(Instance& sInstance)
{
   if(!m_bModified)
      return;

   m_iHealth = m_iPrice * sInstance.m_iVariables;

   if(!m_bSattisfied)
      m_iHealth /= ((1 + sInstance.m_iClauses - m_iSattisfied) * g_fNotSatisfiedPenalty);

   if(m_bSattisfied)
      m_iHealth *= g_fNotSatisfiedPenalty;

   return;
}

void Solution::updateSatisfied(Instance *pInstance)
{
   bool bRes = true;
   bool bClausuleRes;
   int var;

   m_iSattisfied = 0;

   for(int iClausuleIndex = 0; iClausuleIndex < pInstance->m_iClauses; iClausuleIndex++)
   {
      bClausuleRes = false;
      for(int iVariableIndex = 0; iVariableIndex < pInstance->m_vClauses[iClausuleIndex].m_iSize; iVariableIndex++)
      {
         var = pInstance->m_vClauses[iClausuleIndex].m_vVariables[iVariableIndex];
         if(var > 0)
            bClausuleRes |= m_vSelection[(var-1)];
         else
            bClausuleRes |= !m_vSelection[-(var+1)];
      }
      if(!bClausuleRes)
      {
         bRes = false;
         if(g_bBandB)
         {
            m_bSattisfied = bRes;
            return;
         }
      }
      else
      {
         m_iSattisfied++;
      }
   }

   m_bSattisfied = bRes;
   return;
}

void Population::updateSatisfied()
{
   for(int iIndex = 0; iIndex < m_iSize; iIndex++)
   {
      if(m_aGeneration[iIndex].m_bModified)
         m_aGeneration[iIndex].updateSatisfied(&m_sInstance);
   }
}

int strategy_Genetic_optimize(Instance &sInstance)
{
   Solution best;
   uint uiAvgPrice = 0;
   uint uiBestAvgPrice = 0;
   uint uiBestGenerations = 0;
   uint uiBestPopulation = 0;
   float fBestSelectionPressure = 0;
   float fBestMutations = 0;
   float fBestNotSatisfiedPenalty = 0;

   std::cout << std::endl;
   
   // Find best parameters
   for(g_iGenerations = 800; g_iGenerations <= 800; g_iGenerations += 20)
   {
      for(g_iPopulation = 160; g_iPopulation <= 160; g_iPopulation += 10)
      {
         for(g_fSelectionPressure = 0.2; g_fSelectionPressure <= 0.8; g_fSelectionPressure += 0.1)
         {
            for(g_fMutations = 0.005; g_fMutations < 0.5; g_fMutations *= 2)
            {
               for(g_fNotSatisfiedPenalty = 1; g_fNotSatisfiedPenalty < 6; g_fNotSatisfiedPenalty += 1)
               {
                  std::cout << ".";
                  std::cout.flush();
                  
                  uiAvgPrice = 0;
                  for(int i = 0; i < 10; i++)
                  {
                     strategy_Genetic(sInstance);
                     uiAvgPrice += sInstance.m_sResult.m_iPrice;
                  }
                  uiAvgPrice /= 10;

                  if(uiAvgPrice > uiBestAvgPrice)
                  {
                     best = sInstance.m_sResult;
                     uiBestAvgPrice = uiAvgPrice;
                     uiBestGenerations = g_iGenerations;
                     uiBestPopulation = g_iPopulation;
                     fBestSelectionPressure = g_fSelectionPressure;
                     fBestMutations = g_fMutations;
                     fBestNotSatisfiedPenalty = g_fNotSatisfiedPenalty;
                  }
               }
            }
         }
      }
   }

   sInstance.m_sResult = best;
   std::cout << "Best result for " << sInstance.m_iIndex << ":" << std::endl <<
                "average m_iPrice: " << uiBestAvgPrice << std:: endl << "correct m_iPrice: " << g_mSolutions[sInstance.m_iIndex].m_iPrice << std::endl <<
                "generations: " << uiBestGenerations << std::endl << "Population: " << uiBestPopulation << std::endl << "selection pressure: " << fBestSelectionPressure << std::endl << "mutations: " << fBestMutations << std::endl << "not satisfied penalty: " << fBestNotSatisfiedPenalty << std::endl;

   return 0;
}
