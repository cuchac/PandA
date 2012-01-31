#ifndef __MAIN_H__
#define __MAIN_H__


#include <vector>
#include <map>
#include <sys/types.h>


/**
Number of visited states global variable
*/
extern uint g_uiStates;

/**
This represents the item in Clause.
*/
class Variable {
public:
   Variable(int i, int w): m_iIindex(i), m_iWeight(w) {};
   Variable(): m_iWeight(0), m_iIindex(0) {};

   // Members
   int   m_iIindex;
   int   m_iWeight;
};

typedef std::vector<bool> selection_t;

/**
Represents solution of sat problem.
*/
class Instance;

class Solution
{
public:
   Solution(): m_bModified(true), m_bSattisfied(false), m_iSattisfied(0), m_iPrice(0), m_iHealth(0) {};
   Solution(int iSize): m_bModified(true), m_bSattisfied(false), m_iSattisfied(0), m_iPrice(0), m_iHealth(0), m_vSelection(iSize) {};
public:
   static bool sorterByHealth(const Solution *i1, const Solution *i2) {g_uiStates++; return i1->m_iHealth > i2->m_iHealth;};
   static bool sorterByPrice(const Solution *i1, const Solution *i2) {g_uiStates++; return i1->m_iPrice > i2->m_iPrice;};

   void calcHealth(Instance& sInstance);
   void add(Variable &variable) {if(!m_vSelection[variable.m_iIindex]) {m_bModified=true; m_iPrice += variable.m_iWeight; m_vSelection[variable.m_iIindex] = true;};};
   void del(Variable &variable) {if(m_vSelection[variable.m_iIindex]) {m_bModified=true; m_iPrice -= variable.m_iWeight; m_vSelection[variable.m_iIindex] = false;};};
   void swap(Variable &variable) {if(m_vSelection[variable.m_iIindex]) del(variable); else add(variable);};
   void set(Variable &variable, bool bSet) {if(bSet) add(variable); else del(variable);};
   void updateSatisfied(Instance *pInstance);

public:
   selection_t m_vSelection;

   bool  m_bModified;
   bool  m_bSattisfied;
   int   m_iSattisfied;
   int   m_iPrice;
   int   m_iHealth;
};

class Clause
{
public:
   Clause(): m_iSize(3), m_vVariables(3){};

   std::vector< int > m_vVariables;

public:
   int   m_iSize;
};

/**
This represents specific instance of problem with restrictions.
Instance keeps its calculated solution in result member.
*/
class Instance {
public:
   Instance(): m_iVariables(0), m_iClauses(0) {};

public:
   // Members
   int  m_iIndex;
   int  m_iVariables;
   int  m_iClauses;

   std::vector<Variable> m_vVariables;
   std::vector< Clause > m_vClauses;

   // Computed solution
   Solution m_sResult;
};

/**
Map of instances indexed by unique ID
*/
extern std::map<int, Instance> g_mInstances;
typedef std::map<int, Instance> instances_t;

/**
Map of solutions indexed by unique ID
*/
extern std::map<int, Solution> g_mSolutions;
typedef std::map<int, Solution> solutions_t;

/**
Set of flags controlling execution parameters
*/
#define  FLAG_DEBUG   0x01
#define  FLAG_TIME    0x02
#define  FLAG_CHECK   0x04
#define  FLAG_SUMMARY 0x08

/**
Execution flags global variable
*/
extern uint g_uiFlags;

/**
Number of iterations global variable
*/
extern uint g_uiInterations;


#endif /* __MAIN_H__ */
