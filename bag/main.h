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
This represents the item in bag.
*/
struct item_t {
   item_t(int i, int p, int w): index(i), price(p), weight(w) {};
   item_t(): index(0), price(0), weight(0) {};

   // Sorting functors
   static bool sorter_by_price(const struct item_t &i1, const struct item_t &i2) {g_uiStates++; return i1.price > i2.price;};
   static bool sorter_by_weight(const struct item_t &i1, const struct item_t &i2) {g_uiStates++; return i1.weight > i2.weight;};
   static bool sorter_by_id(const struct item_t &i1, const struct item_t &i2) {g_uiStates++; return i1.index < i2.index;};
   static bool sorter_by_quality(const struct item_t &i1, const struct item_t &i2) {g_uiStates++; return ((double)i1.price / i1.weight) > ((double)i2.price / i2.weight);};

   // Members
   int  index;
   int  price;
   int  weight;
};

typedef struct item_t item;
typedef std::vector<item> items_t;
typedef std::vector<bool> selection_t;

/**
Represents solution of bag problem.
*/
struct Solution {
   Solution(): price(0), weight(0), index(0) {};

   // Helper functions
   void  add(item &item) {if(!selection[item.index]) {price += item.price; weight += item.weight; selection[item.index] = true;};};
   void  del(item &item) {if(selection[item.index]) {price -= item.price; weight -= item.weight; selection[item.index] = false;};};

   // Members
   int  price;
   int  weight;
   int  index;

   std::vector<bool> selection;
};

typedef struct Solution solution;

/**
This represents specific instance of problem with restrictions.
Instance keeps its calculated solution in result member.
*/
struct Instance {
   Instance(): capacity(0), index(0) {};

   // Members
   int  capacity;
   int  index;

   std::vector<item> items;

   // Computed solution
   solution result;
};

typedef struct Instance instance;

/**
Map of instances indexed by unique ID
*/
extern std::map<int, instance> g_mInstances;
typedef std::map<int, instance> instances_t;

/**
Map of solutions indexed by unique ID
*/
extern std::map<int, solution> g_mSolutions;
typedef std::map<int, solution> solutions_t;

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
