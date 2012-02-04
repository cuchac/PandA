/******************************************/
/* KYBLE V1.0       PAA  1999     kyble.c */
/* Reseni zobecneneho problemu dvou kyblu */
/* Copyright (c) 1999 by KP CVUT-FEL      */
/******************************************/

#include "buckets.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* #define DEBUG            /* Control prints */
/* #define DEBUGO           /* Control prints - operations */
/* #define DEBUGQ           /* Control prints - queue */
#define HARDERR          /* Pri preteceni fronty program skonci */

#define MAXVERTEX 100000  /* max. pocet uzlu grafu stav. prostoru */
#define MAXQUEUE  30000   /* max. pocet rozpracovanych uzlu */
#define MAXBUCKET 5       /* run-time max. pocet kyblu */

unsigned full_buckets[MAXBCKTS];  /* objem jednotl. kyblu */
unsigned final_buckets[MAXBCKTS]; /* koncovy stav */
vertex vertices[MAXVERTEX];        /* souvisly seznam uzlu */
unsigned ffree=1;                  /* ukazatel na prvni volne misto v poli uzlu */
unsigned queue[MAXQUEUE];          /* fronta rozpracovanych uzlu */
unsigned qcnt=0;                   /* pocet prvku ve fronte */
unsigned first=0,last=0;           /* prvni, posledni prvek fronty */
FILE* trace;                       /* ulozeni prubehu vypoctu */
FILE* fv;                          /* ulozeni reseni */
FILE* data;                        /* zadani instance */
// struct time t;                     /* cas vypoctu */
struct tm * t_m;
time_t t;


/* Konvence: prvni prvek v poli vertices (tj. 0) musi zustat prazdny */

void set_final_buckets(void) {
final_buckets[0]=14;
final_buckets[1]=10;
final_buckets[2]=8;
final_buckets[3]=1;
final_buckets[4]=0;
final_buckets[5]=3;
final_buckets[6]=3;
final_buckets[7]=3;
final_buckets[8]=3;
final_buckets[9]=3; /* */
}

void set_full_buckets(void) {
full_buckets[0]=15;
full_buckets[1]=12;
full_buckets[2]=8;
full_buckets[3]=4;
full_buckets[4]=6;
full_buckets[5]=3;
full_buckets[6]=3;
full_buckets[7]=3;
full_buckets[8]=3;
full_buckets[9]=3; /* */
}


void putvertex(unsigned b) {
/* ulozi novy stav do grafu */
(vertices+ffree)->backptr=b;
if (b) (vertices+ffree)->count=(vertices+b)->count+1;
else (vertices+ffree)->count=0;
(vertices+ffree)->hits=1;
if ((ffree++)>MAXVERTEX) {
   printf("PUTVERTEX: MAXIMUM NUMBER OF VERTICES EXCEEDED \n");
   fprintf(trace,"PUTVERTEX: MAXIMUM NUMBER OF VERTICES EXCEEDED \n");
   exit(1);
   }
}

//unsigned set_initial_buckets(void) {
//unsigned b=1;
//ffree=1; /* pro jistotu */
//(vertices+b)->backptr=0;      /* pocatecni stav nema predchudce */
//(vertices+b)->bucket[0]=0;
//(vertices+b)->bucket[1]=0;
//(vertices+b)->bucket[2]=0;
//(vertices+b)->bucket[3]=0;
//(vertices+b)->bucket[4]=0;
//(vertices+b)->bucket[5]=0;
//(vertices+b)->bucket[6]=0;
//(vertices+b)->bucket[7]=0;
//(vertices+b)->bucket[8]=0;
//(vertices+b)->bucket[9]=0; /* */
//putvertex(0);
//return b;
//}

#include "data.h"

unsigned isempty(unsigned b, unsigned which) {
/* zjisti, jestli dany kybl je prazdny */
return(!((vertices+b)->bucket[which]));
}

unsigned isfull(unsigned b, unsigned which) {
/* zjisti, jestli dany kybl je plny */
return(((vertices+b)->bucket[which])==full_buckets[which]);
}


unsigned empty(unsigned bk, unsigned which) {
/* Vyleje kybl, jehoz poradi je urceno cislem which */
unsigned a=0;
vertex *vert, *buckets;
if (which > MAXBUCKET) {
   printf("EMPTY: INVALID BUCKET NUMBER\n");
   fprintf(trace,"EMPTY: INVALID BUCKET NUMBER\n");
   exit(1);
   }
if (bk > MAXVERTEX) {
   printf("EMPTY: MAXIMUM NUMBER OF VERTICES EXCEEDED\n");
   fprintf(trace,"EMPTY: MAXIMUM NUMBER OF VERTICES EXCEEDED\n");
   exit(1);
   }
buckets=(vertices+bk);
#ifdef DEBUGO
printf("EMPTY: ENTRY B %2i V %2i \n",which,buckets->bucket[which]);
fprintf(trace,"EMPTY: ENTRY B %2i V %2i \n",which,buckets->bucket[which]);
#endif
vert=(vertices+ffree);
vert->backptr=bk;
vert->oper=0;
vert->count=0;
vert->which1=which;
vert->which2=0;
for(a=0;a<MAXBUCKET;a++) vert->bucket[a]=buckets->bucket[a];
vert->bucket[which]=0;
#ifdef DEBUGO
buckets=(vertices+ffree);
printf("EMPTY: EXIT  B %2i V %2i \n",which,buckets->bucket[which]);
fprintf(trace,"EMPTY: EXIT  B %2i V %2i \n",which,buckets->bucket[which]);
#endif
return ffree;
}

unsigned fill(unsigned bk, unsigned which) {
/* Naplni kybl, jehoz poradi je urceno cislem which */
unsigned a=0;
vertex *vert, *buckets;
if (which > MAXBUCKET) {
   printf("FILL: INVALID BUCKET NUMBER\n");
   fprintf(trace,"FILL: INVALID BUCKET NUMBER\n");
   exit(1);
   }
if (bk > MAXVERTEX) {
   printf("FILL: MAXIMUM NUMBER OF VERTICES EXCEEDED\n");
   fprintf(trace,"FILL: MAXIMUM NUMBER OF VERTICES EXCEEDED\n");
   exit(1);
   }
buckets=(vertices+bk);
#ifdef DEBUGO
printf("FILL:  ENTRY B %2i V %2i \n",which,buckets->bucket[which]);
fprintf(trace,"FILL:  ENTRY B %2i V %2i \n",which,buckets->bucket[which]);
#endif
vert=(vertices+ffree);
vert->backptr=bk;
vert->oper=1;
vert->count=0;
vert->which1=which;
vert->which2=0;
for(a=0;a<MAXBUCKET;a++) vert->bucket[a]=buckets->bucket[a];
vert->bucket[which]=full_buckets[which];
#ifdef DEBUGO
buckets=(vertices+ffree);
printf("FILL:  EXIT  B %2i V %2i \n",which,buckets->bucket[which]);
fprintf(trace,"FILL:  EXIT  B %2i V %2i \n",which,buckets->bucket[which]);
#endif
return ffree;
}

unsigned pour(unsigned bk, unsigned which1, unsigned which2) {
/* Preleje obsah kyblu do druheho */
/* which2=which1; which1=0;*/
unsigned a=0,pom1=0,pom2=0;
vertex *vert, *buckets;
if (which1 > MAXBUCKET) {
   printf("POUR: INVALID BUCKET NUMBER\n");
   fprintf(trace,"POUR: INVALID BUCKET NUMBER\n");
   exit(1);
   }
if (bk > MAXVERTEX) {
   printf("POUR: MAXIMUM NUMBER OF VERTICES EXCEEDED\n");
   fprintf(trace,"POUR: MAXIMUM NUMBER OF VERTICES EXCEEDED\n");
   exit(1);
   }

buckets=(vertices+bk);
#ifdef DEBUGO
printf("POUR:  ENTRY B %2i V %2i B %2i V %2i \n",which1,buckets->bucket[which1],which2,buckets->bucket[which2]);
fprintf(trace,"POUR:  ENTRY B %2i V %2i B %2i V %2i \n",which1,buckets->bucket[which1],which2,buckets->bucket[which2]);
#endif
vert=(vertices+ffree);
vert->backptr=bk;
vert->oper=2;
vert->count=0;
vert->which1=which1;
vert->which2=which2;
for(a=0;a<MAXBUCKET;a++) vert->bucket[a]=buckets->bucket[a];
/* vert->bucket[which2]=(vert->bucket[which1]>vert->bucket[which2])?full_buckets[which2]:vert->bucket[which1]; /* Old version */
pom1=full_buckets[which2]-vert->bucket[which2]; /* volne misto v druhem kyblu */
pom2=min(vert->bucket[which1],pom1);            /* bud se omezuje volnym mistem v druhem kyblu, nebo mnozstvim vody v prvnim kyblu */
vert->bucket[which1]=vert->bucket[which1]-pom2; /* odlit */
vert->bucket[which2]=vert->bucket[which2]+pom2; /* nalit */
#ifdef DEBUGO
buckets=(vertices+ffree);
printf("POUR:  EXIT  B %2i V %2i B %2i V %2i \n",which1,buckets->bucket[which1],which2,buckets->bucket[which2]);
fprintf(trace,"POUR:  EXIT  B %2i V %2i B %2i V %2i \n",which1,buckets->bucket[which1],which2,buckets->bucket[which2]);
#endif
return ffree;
}

unsigned check(unsigned b) {
/* Zkontroluje, jestli dany stav je pozadovane reseni */
unsigned a=0, flag=1;
vertex *buckets;
buckets=(vertices+b);
for(a=0;a<MAXBUCKET;a++) if (buckets->bucket[a]!=final_buckets[a]) flag=0;
return flag;
}

unsigned initcheck(unsigned b) {
/* Zkontroluje, jestli dany stav je pripustny stav stavoveho prostoru */
unsigned a=0, flag=1;
vertex *buckets;
buckets=(vertices+b);
for(a=0;a<MAXBUCKET;a++) if (buckets->bucket[a]>full_buckets[a]) flag=0;
return flag; /* 1..OK, 0..FAIL */
}

unsigned compare(unsigned b, unsigned bk) {
/* Zkontroluje, jestli jsou uzly shodne */
unsigned a=0, flag=1;
vertex *buckets1, *buckets2;
buckets1=(vertices+b);
buckets2=(vertices+bk);
if (b==bk) return 1; /* New version */
for(a=0;a<MAXBUCKET;a++) if (buckets1->bucket[a]!=buckets2->bucket[a]) flag=0;
return flag;
}

void assign_read(FILE **F, const char *Name) {
/* otevre soubor na vstup */
if ((*F = fopen(Name,"r")) == NULL) {
   printf("ASSIGN_READ: UNABLE TO OPEN FILE %s\n", Name);
   exit(1);
   }
}

void assign_rewrite(FILE **F, const char *Name) {
/* otevre soubor na vystup */
if ((*F = fopen(Name,"wt")) == NULL) {
   printf("ASSIGN_REWRITE: UNABLE TO OPEN FILE %s\n", Name);
   exit(1);
   }
}

int eval_heuristic(unsigned b) {
int i,j,val;
i=j=val=0;

vertex vert=vertices[b];
for(i=0; i<MAXBUCKET; i++) {
   if(vert.bucket[i]==final_buckets[i]) {
      val+=2;
   }
   else {
      for(j=0; j<MAXBUCKET; j++) {
         if(vert.bucket[i]==final_buckets[j]) {
            val+=1;
            break;
         }
      }
   }
}

return val;
}

void enqueue(unsigned b) {
/* vlozi dany uzel do fronty k dalsimu zpracovani */
static unsigned cnt=0;
#ifdef DEBUGB
unsigned a=0;
#endif
cnt++;
#ifdef DEBUGQ
printf("ENQUEUE: PASS %i MEMBERS %i\n",cnt,qcnt);
fprintf(trace,"ENQUEUE: PASS %i MEMBERS %i\n",cnt,qcnt);
#endif
#ifdef DEBUGB
printf("ENQUEUE: BUCKETS  ");
fprintf(trace,"ENQUEUE: BUCKETS  ");
for(a=0;a<MAXBUCKET;a++) {
   printf("%i ",(vertices+b)->bucket[a]);
   fprintf(trace,"%i ",(vertices+b)->bucket[a]);
   }
printf("\n");
fprintf(trace,"\n");
#endif
if (((last+1)%MAXQUEUE)==first) {
   printf("ENQUEUE: QUEUE IS FULL   PASS %i\n",cnt);
   fprintf(trace,"ENQUEUE: QUEUE IS FULL    PASS %i\n",cnt);
#ifdef HARDERR
   exit(1);
#else
   return;
#endif
   }
else qcnt++;
queue[last]=b;
last=(last+1)%MAXQUEUE;
}

void enqueue_heuristic(unsigned b) {
/* vlozi dany uzel do fronty k dalsimu zpracovani */
static unsigned cnt=0;
#ifdef DEBUGB
unsigned a=0;
#endif
cnt++;
#ifdef DEBUGQ
printf("ENQUEUE: PASS %i MEMBERS %i\n",cnt,qcnt);
fprintf(trace,"ENQUEUE: PASS %i MEMBERS %i\n",cnt,qcnt);
#endif
#ifdef DEBUGB
printf("ENQUEUE: BUCKETS  ");
fprintf(trace,"ENQUEUE: BUCKETS  ");
for(a=0;a<MAXBUCKET;a++) {
   printf("%i ",(vertices+b)->bucket[a]);
   fprintf(trace,"%i ",(vertices+b)->bucket[a]);
   }
printf("\n");
fprintf(trace,"\n");
#endif
if (((last+1)%MAXQUEUE)==first) {
   printf("ENQUEUE: QUEUE IS FULL   PASS %i\n",cnt);
   fprintf(trace,"ENQUEUE: QUEUE IS FULL    PASS %i\n",cnt);
#ifdef HARDERR
   exit(1);
#else
   return;
#endif
   }
else qcnt++;
vertices[b].heuristic_val=eval_heuristic(b);
if(last==first) {
   last=(last+1)%MAXQUEUE;
   queue[first]=b;
}
else {
   unsigned int pos2=first;
   first=(first+MAXQUEUE-1)%MAXQUEUE;
   queue[first]=b;
   unsigned int pos1=first;
   while((vertices[queue[pos2]].heuristic_val>=vertices[b].heuristic_val)&&(pos2!=last)) {
      queue[pos1]=queue[pos2];
      queue[pos2]=b;
      pos1=pos2;
      pos2=(pos2+1)%MAXQUEUE;
   }
}
}

unsigned dequeue(void) {
/* vybere uzel z fronty k dalsimu zpracovani */
static unsigned cnt=0;
unsigned b=0;
cnt++;
if (qcnt) qcnt--;       /* osetreni podteceni */
#ifdef DEBUGQ
printf("DEQUEUE: PASS %i MEMBERS %i\n",cnt,qcnt);
fprintf(trace,"DEQUEUE: PASS %i MEMBERS %i\n",cnt,qcnt);
#endif
if (first==last) {
   printf("DEQUEUE: QUEUE IS EMPTY    PASS %i\n",cnt);
   fprintf(trace,"DEQUEUE: QUEUE IS EMPTY    PASS %i\n",cnt);
   return(0);
   }
b=queue[first];
first=(first+1)%MAXQUEUE;
if (b==0) {
   printf("DEQUEUE: FATAL ERROR    PASS %i\n",cnt);
   fprintf(trace,"DEQUEUE: FATAL ERROR    PASS %i\n",cnt);
   exit(1);
   }
return(b);
}

void save(unsigned temp, unsigned prev) {
/* zapise postup operaci pro nalezeni reseni do souboru */
/* prev je primy predchudce uzlu temp pro prave provadenou operaci */
/* je to kvuli osetreni vice zpusobu dosazeni reseni */
unsigned b=temp,c=0;
unsigned bb=0;
static unsigned cnt=0;
cnt++;
if ((temp!=prev)&&(prev!=(vertices+temp)->backptr)) {
   printf("SAVE: DUPLICATE SOLUTION   #%i\n",cnt);
   fprintf(fv,"SAVE: DUPLICATE SOLUTION   #%i\n",cnt);
   }

for(bb=0;bb<MAXBUCKET;bb++) {
   printf("%2i ",(vertices+temp)->bucket[bb]);
   fprintf(fv,"%2i ",(vertices+temp)->bucket[bb]);
   }
printf("\n");
fprintf(fv,"\n");

/* b=(vertices+b)->backptr; /* Old version */
/* if (!(b=(vertices+b)->backptr)) b=prev; /* asi to neni ono */

while (b!=0) {
   c++;
   if ((vertices+b)->backptr!=0) { /* prvni uzel je pocatecni stav a neni pro nej definovana operace */
      printf("#%2i %2i OPER %i  WHICH1 %i  ",cnt,c,(vertices+b)->oper,(vertices+b)->which1);
      if ((vertices+b)->oper==2) printf("WHICH2 %i   #%i ",(vertices+b)->which2,cnt);
      else printf("           #%i ",cnt);
      fprintf(fv,"#%2i %2i OPER %i  WHICH1 %i  ",cnt,c,(vertices+b)->oper,(vertices+b)->which1);
      if ((vertices+b)->oper==2) fprintf(fv,"WHICH2 %i   #%i ",(vertices+b)->which2,cnt);
      else fprintf(fv,"           #%i ",cnt);
      for(bb=0;bb<MAXBUCKET;bb++) {
         printf("%2i ",(vertices+b)->bucket[bb]);
         fprintf(fv,"%2i ",(vertices+b)->bucket[bb]);
         }
      printf("\n");
      fprintf(fv,"\n");
      }
   b=(vertices+b)->backptr;
   }
}

char* searched;

void putnew(unsigned par) {
unsigned i,k;

k=(vertices+ffree)->bucket[0];
for(i=1; i<MAXBUCKET; i++)
   k=k*(full_buckets[i]+1)+(vertices+ffree)->bucket[i];
if(searched[k])
   return;
searched[k]=1;
putvertex(par);     
enqueue(ffree-1);
}

void putnew_heuristic(unsigned par) {
unsigned i,k;

k=(vertices+ffree)->bucket[0];
for(i=1; i<MAXBUCKET; i++)
   k=k*(full_buckets[i]+1)+(vertices+ffree)->bucket[i];
if(searched[k])
   return;
searched[k]=1;
putvertex(par);     
enqueue_heuristic(ffree-1);
}

void search(void) {
unsigned i,j,k,par,all=1;

for(i=0; i<MAXBUCKET; i++)
   all*=(full_buckets[i]+1);
searched=(char *)malloc(all);
memset(searched,0,all);
k=(vertices+ffree-1)->bucket[0];
for(i=1; i<MAXBUCKET; i++)
   k=k*(full_buckets[i]+1)+(vertices+ffree-1)->bucket[i];
searched[k]=1;
enqueue(ffree - 1);

while(1) {
   par = dequeue();
   if (par == 0)
      break;
   if (check(par)) {
      save(par, (vertices + par)->backptr);
      break;
   }

   for(i = 0; i < MAXBUCKET; i++) {
      if (!isfull(par, i)) {
         fill(par,i);
         putnew(par);
         for(j = 0; j < MAXBUCKET; j++) {
            if (i != j && !isempty(par, j)) {
               pour(par, j, i);
               putnew(par);
            }
         }
      }

      if (!isempty(par, i)) {
         empty(par,i);
         putnew(par);
      }
   }
}

free(searched);

printf("SEARCH: DONE\n");
fprintf(trace,"SEARCH: DONE\n");
}

void search_heuristic(void) {
unsigned i,j,k,par,all=1;

for(i=0; i<MAXBUCKET; i++)
   all*=(full_buckets[i]+1);
searched=(char *)malloc(all);
memset(searched,0,all);
k=(vertices+ffree-1)->bucket[0];
for(i=1; i<MAXBUCKET; i++)
   k=k*(full_buckets[i]+1)+(vertices+ffree-1)->bucket[i];
searched[k]=1;
enqueue_heuristic(ffree - 1);

while(1) {
   par = dequeue();
   if (par == 0)
      break;
   if (check(par)) {
      save(par, (vertices + par)->backptr);
      break;
   }

   for(i = 0; i < MAXBUCKET; i++) {
      if (!isfull(par, i)) {
         fill(par,i);
         putnew_heuristic(par);
         for(j = 0; j < MAXBUCKET; j++) {
            if (i != j && !isempty(par, j)) {
               pour(par, j, i);
               putnew_heuristic(par);
            }
         }
      }

      if (!isempty(par, i)) {
         empty(par,i);
         putnew_heuristic(par);
      }
   }
}

free(searched);

printf("SEARCH: DONE\n");
fprintf(trace,"SEARCH: DONE\n");
}

void dump_vertices(void) {
unsigned a=0,b=0;
for (a=1;a<ffree;a++) {
   printf("#%2i BUCKETS: ",a);
   fprintf(trace,"#%2i BUCKETS: ",a);
   for(b=0;b<MAXBUCKET;b++) {
      printf("%2i ",(vertices+a)->bucket[b]);
      fprintf(trace,"%2i ",(vertices+a)->bucket[b]);
      }
   printf(" DIST: %2i  HITS: %2i\n",(vertices+a)->count,(vertices+a)->hits);
   fprintf(trace," DIST: %2i  HITS: %2i\n",(vertices+a)->count,(vertices+a)->hits);
   }
}

void save_vertices(void) {
FILE *mdl;
unsigned b=0;

if ((mdl = fopen("source.bin","wb")) == NULL) {
   printf("SAVE_VERTICES: ERROR OPENING FILE");
   exit(1);
}
printf("Writing space \n");
fprintf(trace,"Writing space \n");

if (fwrite(&ffree,sizeof(int),1,mdl)==-1) {
   printf("SAVE_VERTICES: ERROR WRITING VERTEX COUNT");
   exit(1);
}

for(b=0;b<ffree;b++) {
   if (fwrite((vertices+b),sizeof(vertex),1,mdl)==-1) {
      printf("SAVE_VERTICES: ERROR WRITING VERTEX");
      exit(1);
   }
}

fclose(mdl);
}

void print_help(char* name) {
   printf("Usage: %s [options] <instance> <bfs|heuristic>\n", name);
   printf("Instance:\n");
   printf("  11      Instance 1.1\n");
   printf("  12      Instance 1.2\n");
   printf("  13      Instance 1.3\n");
   printf("  14      Instance 1.4\n");
   printf("  21      Instance 2.1\n");
   printf("  22      Instance 2.2\n");
   printf("  23      Instance 2.3\n");
   printf("  24      Instance 2.4\n");
   printf("  25      Instance 2.5\n");
   printf("  31      Instance 3.1\n");
   printf("  32      Instance 3.2\n");
   printf("  33      Instance 3.3\n");
   printf("  34      Instance 3.4\n");
   printf("  35      Instance 3.5\n");
   printf("  36      Instance 3.6\n");
   printf("Options:\n");
}

int main(int argc, char **argv) {
/* Reseni zobecneneho problemu dvou kyblu */
unsigned a,b,c,h;
a=b=c=0;
h=-1;

if(argc<3) {
   print_help(argv[0]);
   exit(1);
}

assign_rewrite(&trace,"trace.txt");
assign_rewrite(&fv,"solution.txt");
//assign_read(&data,argv[1]);

b=set_initial_buckets();

switch(atoi(argv[1])) {
   case 11:
      set_final_buckets_1_1();
      break;
   case 12:
      set_final_buckets_1_2();
      break;
   case 13:
      set_final_buckets_1_3();
      break;
   case 14:
      set_final_buckets_1_4();
      break;
   case 21:
      set_final_buckets_2_1();
      break;
   case 22:
      set_final_buckets_2_2();
      break;
   case 23:
      set_final_buckets_2_3();
      break;
   case 24:
      set_final_buckets_2_4();
      break;
   case 25:
      set_final_buckets_2_5();
      break;
   case 31:
      set_final_buckets_3_1();
      break;
   case 32:
      set_final_buckets_3_2();
      break;
   case 33:
      set_final_buckets_3_3();
      break;
   case 34:
      set_final_buckets_3_4();
      break;
   case 35:
      set_final_buckets_3_5();
      break;
   case 36:
      set_final_buckets_3_6();
      break;
   default:
      printf("MAIN: UNKNOWN INSTANCE %s\n",argv[1]);
      fprintf(trace,"MAIN: UNKNOWN INSTANCE %s\n",argv[1]);
      exit(1);
}

if(!strcmp(argv[2],"bfs")) h=0;
if(!strcmp(argv[2],"heuristic")) h=1;

if(h==-1) {
   printf("MAIN: UNKNOWN ALGORITHM %s\n",argv[2]);
   fprintf(trace,"MAIN: UNKNOWN ALGORITHM %s\n",argv[2]);
   exit(1);
}

//set_full_buckets();
//b=set_initial_buckets();
//set_final_buckets();
//if (!initcheck(b)) {
//   printf("MAIN: INITIAL STATE IS INVALID\n");
//   fprintf(trace,"MAIN: INITIAL STATE IS INVALID\n");
//   exit(1);
//   }

t = time(&t);
t_m = localtime(&t);

printf("%2i:%02i  MAIN: Starting search...\n",t_m->tm_hour,t_m->tm_min);
fprintf(trace,"%2i:%02i  MAIN: Starting search...\n",t_m->tm_hour,t_m->tm_min);

if(h==0) search(); /* uplne prochazeni stav. prostoru */
if(h==1) search_heuristic();

t = time(&t);
t_m = localtime(&t);

printf("%2i:%02i  MAIN: DONE AFTER EXPLORING %i VERTICES \n\n",t_m->tm_hour,t_m->tm_min,ffree-1);
fprintf(trace,"%2i:%02i  MAIN: DONE AFTER EXPLORING %i VERTICES \n",t_m->tm_hour,t_m->tm_min,ffree-1);
//printf("\n\n\n\n");

//dump_vertices();
//save_vertices(); /* ulozeni stav. prostoru */

fclose(trace);
fclose(fv);
//fclose(data);

}
