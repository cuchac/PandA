#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#include "main.h"
#include "util.h"
#include "strategy.h"

instances_t g_mInstances;
solutions_t g_mSolutions;
uint        g_uiFlags = 0;
uint        g_uiInterations = 1;

void print_help(char * sProgramName);

int main(int argc, char **argv) {
   // Parse command line
   char option;
   while ((option = getopt(argc, argv, "+ctdsn:")) != -1) {
      switch (option) {
         case 'c':
            g_uiFlags |= FLAG_CHECK;
            break;
         case 't':
            g_uiFlags |= FLAG_TIME;
            break;
         case 'd':
            g_uiFlags |= FLAG_DEBUG;
            break;
         case 's':
            g_uiFlags |= FLAG_SUMARY;
            break;
         case 'n':
            g_uiInterations = atoi(optarg);
            std::cout << "Setting iterations to: " << g_uiInterations << std::endl;
            break;
         default:
            print_help(argv[0]);
            exit(1);
      }
   }

   argc -= optind-1;
   argv += optind-1;

   if(argc < 3)
   {
      print_help(argv[0]);
      exit(1);
   }

   // Load DATA from file
   std::ifstream dataFile;
   dataFile.open(argv[2]);
   if(!dataFile.good())
   {
      std::cout << "Cannot open file: " << argv[2] << std::endl;
      exit(2);
   }
   else
   {
      if(!loadData(dataFile))
      {
         std::cout << "Error loading file: " << argv[2] << std::endl;
         exit(4);
      }
   }

   // Load results from file
   if(argc >= 4)
   {
      std::ifstream dataFile;
      dataFile.open(argv[3]);
      if(!dataFile.good())
      {
         std::cout << "Cannot open file: " << argv[3] << std::endl;
         exit(3);
      }
      else
      {
         if(!loadSolutions(dataFile))
         {
            std::cout << "Error loading file: " << argv[2] << std::endl;
            exit(4);
         }
      }
   }

   // Choose correct strategy
   int iStrategy = atoi(argv[1]);
   strategy_function fStrategy;

   switch(iStrategy)
   {
      case 1:
         fStrategy = strategy_BrutalForce;
         break;

      case 2:
         fStrategy = strategy_Greedy;
         break;

      case 3:
         fStrategy = strategy_Dynamic;
         break;

      case 4:
         fStrategy = strategy_BandB;
         break;
			
		case 5:
         fStrategy = strategy_Genetic;
         break;

      default:
         std::cout << "Unknown solution: " << argv[1] << std::endl;
         exit(5);
         break;
   }
   
   // Init random generator
	srand(time(NULL));

   // Initiate time measurement
   timespec tStartTime;
   clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tStartTime);

   // Statistical variables
   uint  uiInstancesSolvedTotal = 0;
   uint  uiInstancesSolvedCorrectly = 0;
   double dErrorSum = 0;

   for(uint uiIteration = 0; uiIteration < g_uiInterations; uiIteration++)
   {
      for(instances_t::iterator it = g_mInstances.begin(); it != g_mInstances.end(); it++)
      {
         it->second.result.price = it->second.result.weight = 0;
         fStrategy(it->second);

         uiInstancesSolvedTotal++;
         double dError = check_instance(it->second, g_mSolutions[it->first]);
         if(dError == 0)
            uiInstancesSolvedCorrectly++;
         dErrorSum += dError;
      }
   }

   // Print time informations
   if(g_uiFlags & FLAG_TIME)
   {
      timespec tEndTime;
      clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tEndTime);
      std::cout << "Time measurement: " << (((double) (tEndTime.tv_sec - tStartTime.tv_sec) * 1000 + (double)(tEndTime.tv_nsec - tStartTime.tv_nsec) / 1000000)) << "ms" << std::endl;
   }

   // Print solutions sumary
   if(g_uiFlags & FLAG_SUMARY)
   {
      std::cout << "Summary: Instances solved: " << uiInstancesSolvedTotal <<
         ", correctly: " << uiInstancesSolvedCorrectly <<
         ", mean error: " << (dErrorSum / uiInstancesSolvedTotal) << std::endl;
   }

   return 0;
}

void print_help(char * sProgramName)
{
   std::cout << "Usage: " << sProgramName << " [options] <strategy> <data_file> [<solutions_file>]" << std::endl;
   std::cout << "Strategies:" << std::endl;
   std::cout << "\t1      brutal-force (part 1)" << std::endl;
   std::cout << "\t2      simple heurestic (part 1)" << std::endl;
   std::cout << "\t3      dynamic programing (part 3)" << std::endl;
   std::cout << "\t4      branch and bounds (part 3)" << std::endl;
   std::cout << "\t5      heurestic (part 3)" << std::endl;
   std::cout << "\t6      advanced iterative (part 5)" << std::endl << std::endl;
   std::cout << "Options:" << std::endl;
   std::cout << "\t-c     print check of each solution" << std::endl;
   std::cout << "\t-t     print timings" << std::endl;
   std::cout << "\t-d     debug print of algorythm progress" << std::endl;
   std::cout << "\t-s     print summary (very useful)" << std::endl;
   std::cout << "\t-n <x> repeat calculation x times" << std::endl;
}