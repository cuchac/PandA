#include <stdlib.h>
#include <algorithm>
#include "main.h"
#include "util.h"

#define POPULATION_SIZE 30
#define MUTATION_SIZE 5 //How many 'bits' get mutated
#define SELECTION_PRESSURE 20 //how many "kick out" from population
#define OVERLOAD_PENALTY 0.5 //50%

class solution_gen:public solution 
{
public:
	static bool sorterByHealth(const solution_gen &i1, const solution_gen &i2){return i1.m_iHealth > i2.m_iHealth;};
	static bool sorterByPrice(const solution_gen &i1, const solution_gen &i2){return i1.price > i2.price;};
	
	void 	calcHealth(instance& sInstance);
	void 	set(item& item, bool bSet){if(bSet)add(item);else del(item);};
	void  swap(item &item){if(selection[item.index])del(item);else add(item);};

public:
	bool	m_bValid;
	int	m_iHealth;
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
	void combineOne( solution_gen& sParent1, solution_gen& sParent2, solution_gen& sChild);
	void mutateOne( solution_gen& sIndividuum, int iRandom);
	bool isFinished();
	void recalcHealth();
	
public:
	instance &m_sInstance;
	
	int m_iSize;
	std::vector<solution_gen> m_aGeneration;
	int m_iGeneration;
};

int strategy_Genetic(instance &sInstance)
{
	population sPopulation(sInstance, POPULATION_SIZE);
	
	sPopulation.populate();

	while(!sPopulation.isFinished())
	{
		//Update health
		sPopulation.recalcHealth();
		
		//Selection
		sPopulation.select();
		
		//Combination
		sPopulation.combine();

		//Mutation
		sPopulation.mutate();
	}
	
	std::sort(sPopulation.m_aGeneration.begin(), sPopulation.m_aGeneration.end(), solution_gen::sorterByPrice);
	
	for(int iIndex = 0; iIndex < sPopulation.m_iSize; iIndex++)
		if(sPopulation.m_aGeneration[iIndex].weight <= sInstance.capacity)
		{
			sInstance.result = sPopulation.m_aGeneration[iIndex];
			break;
		}
}

population::population ( instance& m_sInstance, int iSize ):m_sInstance(m_sInstance),m_iSize(iSize),m_iGeneration(0)
{
	m_aGeneration.resize(iSize);
}

void population::populate()
{
	for(int iIndex = 0; iIndex < m_iSize; iIndex++)
	{
		m_aGeneration[iIndex].selection.resize(m_sInstance.items.size());
		m_aGeneration[iIndex].m_bValid = true;
	}
		
	for(int iIndex = 0; iIndex < m_iSize*m_sInstance.items.size()/2; iIndex++)
	{
		int iRandom = rand();
		m_aGeneration[iRandom%m_iSize].swap(m_sInstance.items[iRandom%m_sInstance.items.size()]);
	}
}

void population::select()
{
	std::sort(m_aGeneration.begin(), m_aGeneration.end(), solution_gen::sorterByHealth);
	
	int iRankSum = m_iSize * (m_iSize+1) / 2;
	
	// Kill some inhabitants
	for(int iIndex = 0; iIndex < SELECTION_PRESSURE; iIndex++)
	{
		int iRandom = rand() % iRankSum;
		
		int iCurRankSum = 0;
		for(int iRank = m_iSize-1; iRank >=0; iRank--)
		{
			iCurRankSum+=iRank;
			if(iRandom < iCurRankSum)
			{
				m_aGeneration[iRank].m_bValid = false;
				break;
			}
		}
	}
}

void population::combine()
{
	for(int iIndex = 0; iIndex < m_iSize; iIndex++)
	{
		if(m_aGeneration[iIndex].m_bValid)
			continue;
		
		int iFirstParent = rand() % m_iSize;
		int iSecondParent = rand() % m_iSize;
		
		while(iFirstParent==iSecondParent || iFirstParent==iIndex || iSecondParent==iIndex)
		{
			iFirstParent = (iFirstParent + 1) % m_iSize;
			iSecondParent = (iSecondParent +2) % m_iSize;
		}
		
		combineOne(m_aGeneration[iFirstParent], m_aGeneration[iSecondParent], m_aGeneration[iIndex]);
		m_aGeneration[iIndex].m_bValid = true;
	}
}

void population::mutate()
{
	for(int iIndex = 0; iIndex < MUTATION_SIZE; iIndex++)
	{
		int iRandom = rand();
		mutateOne(m_aGeneration[iRandom%m_iSize], iRandom);
	}
}

void population::combineOne ( solution_gen& sFirstParent, solution_gen& sSecondParent, solution_gen& sChild )
{
	int iRandom = rand() % m_sInstance.items.size();
	// Walk through all the gens and combine both parents
	for(int iIndex = 0; iIndex < m_sInstance.items.size(); iIndex++)
		if(iIndex < iRandom)
			sChild.set(m_sInstance.items[iIndex], sFirstParent.selection[iIndex]);
		else
			sChild.set(m_sInstance.items[iIndex], sSecondParent.selection[iIndex]);
}

void population::mutateOne ( solution_gen& sIndividuum, int iRandom )
{
	sIndividuum.swap(m_sInstance.items[iRandom%m_sInstance.items.size()]);
}

bool population::isFinished()
{
	m_iGeneration ++;
	return m_iGeneration > 100;
}

void population::recalcHealth()
{
	for(int iIndex = 0; iIndex < m_iSize; iIndex++)
		m_aGeneration[iIndex].calcHealth(m_sInstance);
}

void solution_gen::calcHealth( instance& sInstance)
{
	m_iHealth = price*1000;
	
	if(weight > sInstance.capacity)
		m_iHealth *= (1-OVERLOAD_PENALTY);
	
// 	if(weight)
// 		m_iHealth -= weight*100;
// 	if(weight)
// 		m_iHealth = price*1000/weight;
// 	else
// 		m_iHealth = 0;
}