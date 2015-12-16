#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>
#include "piano.h"
 
#define POPSIZE 40               /* population size */ //antes 40
#define MAXGEN 8000             /* max. number of generations */
#define PXOVER 0.6               /* probability of crossover */
#define PMUTATION 0.2               // 0.2
#define TRUE 1
#define FALSE 0

int generation;                  /* current generation no. */
int cur_best;                    /* best individual */
int nvars;
FILE *galog;                     /* an output file */
static unsigned char *buf;

typedef struct genotype_ /* genotype (GT), a member of the population */
{
  unsigned char *gene;         /* a string of variables */
  double fitness;            /* GT\'s fitness */
  unsigned char upper;                 /* GT\'s variables upper bound */
  unsigned char lower;                 /* GT\'s variables lower bound */
  double rfitness;           /* relative fitness */
  double cfitness;           /* cumulative fitness */
} GENOTIPO;
GENOTIPO population[POPSIZE+1];    /* population */
GENOTIPO newpopulation[POPSIZE+1]; /* new population; */
                                          /* replaces the */
                                          /* old generation */
/* Declaration of procedures used by this genetic algorithm */
void keep_the_best(void);
double elitist(void);
void select(void);
void crossover(void);
void Xover(int,int);
void swap(unsigned char *, unsigned char *);
void mutate(void);
void report(void);
void swap(unsigned char *x, unsigned char *y);

/***********************************************************/
/* Random value generator: Generates a value within bounds */
/***********************************************************/

static double uniform_deviate(int seed)
{
	return seed * (1.0 / (RAND_MAX + 1.0));
}

static int randval(int low, int high)
{
   int val;

   val = (int)(1 + uniform_deviate(rand()) * (high-low));
   return(val);
}

/***************************************************************/
/* Initialization function: Initializes the values of genes    */
/* within the variables bounds. It also initializes (to zero)  */
/* all fitness values for each member of the population. It    */
/* reads upper and lower bounds of each variable from the      */
/* input file `gadata.txt\'. It randomly generates values       */ 
/* between these bounds for each gene of each genotype in the  */
/* population. The format of the input file `gadata.txt\' is    */
/* var1_lower_bound var1_upper bound                           */
/* var2_lower_bound var2_upper bound ...                       */
/***************************************************************/ 

void initialize(MELODIA *melodia,int penta)
{
   int i,j,c,numcompas,numnotas,v;
   int count=0;

   numcompas=melodia->numcompases;
   for(j=0;j<numcompas;j++)
      count+=(melodia->penta[penta].compas[j].numnotas-melodia->penta[penta].compas[j].numsilencios);                 
   nvars=count;
   buf=(unsigned char *) new unsigned char[nvars];
   for(c=count=0;c<melodia->numcompases;c++){
      numnotas=melodia->penta[penta].compas[c].numnotas;
      for(v=0;v<numnotas;v++){		 
		  if(melodia->penta[penta].compas[c].note[v]==0) continue;   // descarta silencios
          buf[count++]=melodia->penta[penta].compas[c].notekey[v];
	  }
   }  
   for (j = 0; j < POPSIZE+1; j++){
	  newpopulation[j].gene=(unsigned char *) new unsigned char[count];
	  population[j].gene=(unsigned char *) new unsigned char[count];
      population[j].lower=1;
      population[j].upper=5;
      population[j].fitness = 0;
      population[j].rfitness = 0;
      population[j].cfitness = 0;
      for (i = 0; i < nvars; i++)
         population[j].gene[i] = (unsigned char) randval(1,5);
   }
   return;
}

// jordi

static int calccosto(int fo,int fd,int dist)
{
   int f,res;
   
   unsigned char c11[]={0,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30};
   
   unsigned char c12p[]={0,0,0,1,4,8,9,30,30,30,30,30,30,30,30,30};
   unsigned char c12n[]={0,0,0,30,30,30,30,30,30,30,30,30,30,30,30,30};
   unsigned char c21p[]={0,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30};
   unsigned char c21n[]={0,0,0,1,4,8,9,30,30,30,30,30,30,30,30,30};
   
   unsigned char c13p[]={0,4,1,0,0,1,4,8,30,30,30,30,30,30,30,30};
   unsigned char c13n[]={0,0,0,0,10,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c31p[]={0,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30};
   unsigned char c31n[]={0,4,1,0,0,1,4,8,30,30,30,30,30,30,30,30};

   unsigned char c14p[]={0,9,8,4,1,0,0,1,4,30,30,30,30,30,30,30};
   unsigned char c14n[]={0,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c41p[]={0,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30};
   unsigned char c41n[]={0,9,8,4,1,0,0,1,4,30,30,30,30,30,30,30};

   unsigned char c15p[]={0,9,9,8,4,1,0,0,1,4,8,9,9,30,30,30};
   unsigned char c15n[]={0,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c51p[]={0,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30};
   unsigned char c51n[]={0,9,9,8,4,1,0,0,1,4,8,9,9,30,30,30};

   unsigned char c23p[]={0,0,0,30,30,30,30,30,30,30,30,30,30,30,30,30};
   unsigned char c23n[]={0,30,30,10,10,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c32p[]={0,30,30,30,30,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c32n[]={0,0,0,30,30,30,30,30,30,30,30,30,30,30,30,30};

   unsigned char c24p[]={0,4,1,0,0,1,4,30,30,30,30,30,30,30,30,30};
   unsigned char c24n[]={0,30,30,30,10,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c42p[]={0,30,30,10,10,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c42n[]={0,4,1,0,0,1,4,30,30,30,30,30,30,30,30,30};

   unsigned char c25p[]={0,10,8,4,1,0,0,1,30,30,30,30,30,30,30,30};
   unsigned char c25n[]={0,30,30,30,30,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c52p[]={0,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c52n[]={0,10,8,4,1,0,0,1,30,30,30,30,30,30,30,30};

   unsigned char c34p[]={0,0,0,30,30,30,30,30,30,30,30,30,30,30,30,30};
   unsigned char c34n[]={0,30,30,30,30,30,30,30,10,10,10,10,10,10,10,10};
   unsigned char c43p[]={0,30,30,10,10,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c43n[]={0,0,0,30,30,30,30,30,30,30,30,30,30,30,30,30};

   unsigned char c35p[]={0,30,1,0,0,1,4,30,30,30,30,30,30,30,30,30};
   unsigned char c35n[]={0,30,30,30,30,30,30,30,10,10,10,10,10,10,10,10};
   unsigned char c53p[]={0,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c53n[]={0,30,1,0,0,1,4,30,30,30,30,30,30,30,30,30};
   
   unsigned char c45p[]={0,0,0,30,30,30,30,30,30,30,30,30,30,30,30,30};
   unsigned char c45n[]={0,30,30,30,30,30,30,30,30,30,10,10,10,10,10,10};
   unsigned char c54p[]={0,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c54n[]={0,0,0,30,30,30,30,30,30,30,30,30,30,30,30,30};
   
   f=10*fo+fd;
   switch(f){
      case 11:
	  case 22:
      case 33:
	  case 44:
	  case 55:
	     dist=abs(dist);
         res=c11[dist];
		 break;
      case 12:
         if(dist>=0) res=c12p[dist];
		 else res=c12n[-dist];
		 break;
	  case 21:
         if(dist>=0) res=c21p[dist];
		 else res=c21n[-dist];
		 break;
      case 13:
         if(dist>=0) res=c13p[dist];
		 else res=c13n[-dist];
		 break;
	  case 31:
         if(dist>=0) res=c31p[dist];
		 else res=c31n[-dist];
		 break;
      case 14:
         if(dist>=0) res=c14p[dist];
		 else res=c14n[-dist];
		 break;
	  case 41:
         if(dist>=0) res=c41p[dist];
		 else res=c41n[-dist];
		 break;
      case 15:
         if(dist>=0) res=c15p[dist];
		 else res=c15n[-dist];
		 break;
	  case 51:
         if(dist>=0) res=c51p[dist];
		 else res=c51n[-dist];
		 break;
      case 23:
         if(dist>=0) res=c23p[dist];
		 else res=c23n[-dist];
		 break;
	  case 32:
         if(dist>=0) res=c32p[dist];
		 else res=c32n[-dist];
		 break;
      case 24:
         if(dist>=0) res=c24p[dist];
		 else res=c24n[-dist];
		 break;
	  case 42:
         if(dist>=0) res=c42p[dist];
		 else res=c42n[-dist];
		 break;
      case 25:
         if(dist>=0) res=c25p[dist];
		 else res=c25n[-dist];
		 break;
	  case 52:
         if(dist>=0) res=c52p[dist];
		 else res=c52n[-dist];
		 break;
      case 34:
         if(dist>=0) res=c34p[dist];
		 else res=c34n[-dist];
		 break;
	  case 43:
         if(dist>=0) res=c43p[dist];
		 else res=c43n[-dist];
		 break;
      case 35:
         if(dist>=0) res=c35p[dist];
		 else res=c35n[-dist];
		 break;
	  case 53:
         if(dist>=0) res=c53p[dist];
		 else res=c53n[-dist];
		 break;
      case 45:
         if(dist>=0) res=c45p[dist];
		 else res=c45n[-dist];
		 break;
	  case 54:
         if(dist>=0) res=c54p[dist];
		 else res=c54n[-dist];
		 break;
	  default:
		 return -1;
   }
   return res;
}

double fitness(int indi,int penta)
{
	int j,count,fo,fd,fita[30];
	int dist,s,hits,fails;
	double per,fit;


	hits=fails=fit=0;
	count=nvars;
	for(s=0;s<31;s++) fita[s]=0;
	for(j=1;j<count;j++){
       dist=buf[j]-buf[j-1];
	   fd=population[indi].gene[j];
	   fo=population[indi].gene[j-1];	   
	   s=calccosto(fo,fd,dist);
       fita[s]++;
       if(dist>=0 && fd-fo>0) ++hits; //if(dist>=0 && fd-fo>=0) ++hits;
	   else if(dist<0 && fd-fo<0) ++hits;
	   else ++fails;
	}
	per=hits*100.0/(hits+fails);
    fit=(35*per+25*fita[0]+10*fita[1]+9*fita[4])/(fita[8]+2*fita[9]+4*fita[10]+8*fita[30]);
	return fit;
}
   
void evaluate(MELODIA *melodia,int penta)
{
   int mem;   

   for (mem=0;mem<POPSIZE;mem++)
      population[mem].fitness =fitness(mem,penta);
   return;
}
/***************************************************************/
/* Keep_the_best function: This function keeps track of the    */
/* best member of the population. Note that the last entry in  */
/* the array Population holds a copy of the best individual    */
/***************************************************************/
void keep_the_best()
{
int mem;
int i;
cur_best = 0; /* stores the index of the best individual */
for (mem = 0; mem < POPSIZE; mem++)
      {
      if (population[mem].fitness > population[POPSIZE].fitness)
            {
            cur_best = mem;
            population[POPSIZE].fitness = population[mem].fitness;
            }
      }
/* once the best member in the population is found, copy the genes */
for (i = 0; i < nvars; i++)
      population[POPSIZE].gene[i] = population[cur_best].gene[i];
}

/****************************************************************/
/* Elitist function: The best member of the previous generation */
/* is stored as the last in the array. If the best member of    */
/* the current generation is worse then the best member of the  */
/* previous generation, the latter one would replace the worst  */ 
/* member of the current population                             */
/****************************************************************/ 
double elitist()
{
int i;
double best, worst;             /* best and worst fitness values */
int best_mem, worst_mem; /* indexes of the best and worst member */
best = population[0].fitness;
worst = population[0].fitness;
for (i = 0; i < POPSIZE - 1; ++i)
      {
      if(population[i].fitness > population[i+1].fitness)
            {      
            if (population[i].fitness >= best)
                  {
                  best = population[i].fitness;
                  best_mem = i;
                  }
            if (population[i+1].fitness <= worst)
                  {
                  worst = population[i+1].fitness;
                  worst_mem = i + 1;
                  }
            }
      else
            {
            if (population[i].fitness <= worst)
                  {
                  worst = population[i].fitness;
                  worst_mem = i;
                  }
            if (population[i+1].fitness >= best)
                  {
                  best = population[i+1].fitness;
                  best_mem = i + 1;
                  }
            }
      }
/* if best individual from the new population is better than */
/* the best individual from the previous population, then    */
/* copy the best from the new population; else replace the   */
/* worst individual from the current population with the     */
/* best one from the previous generation                     */
if (best >= population[POPSIZE].fitness)
    {
    for (i = 0; i < nvars; i++)
       population[POPSIZE].gene[i] = population[best_mem].gene[i];
    population[POPSIZE].fitness = population[best_mem].fitness;
    }
else
    {
    for (i = 0; i < nvars; i++)
       population[worst_mem].gene[i] = population[POPSIZE].gene[i];
    population[worst_mem].fitness = population[POPSIZE].fitness;
    } 
    return best;
}
/**************************************************************/
/* Selection function: Standard proportional selection for    */
/* maximization problems incorporating elitist model - makes  */
/* sure that the best member survives                         */
/**************************************************************/
void copiastruct(GENOTIPO *ap0,GENOTIPO *ap1)
{
    ap0->cfitness=ap1->cfitness;
    ap0->fitness=ap1->fitness;
	ap0->lower=ap1->lower;
	ap0->upper=ap1->upper;
	ap0->rfitness=ap1->rfitness;

	memcpy((void *) ap0->gene,(void *) ap1->gene,(size_t) nvars);
//
 //   for(h=0;h<nvars;h++)
 //      ap0->gene[h] = ap1->gene[h];      

   return;
}

void select0(void)
{
int mem,i,j;
unsigned int r; 
double sum = 0;
double p; 
/* find total fitness of the population */
for (mem = 0; mem < POPSIZE; mem++)
      {
      sum += population[mem].fitness;
      }
/* calculate relative fitness */
for (mem = 0; mem < POPSIZE; mem++)
      {
      population[mem].rfitness =  100.0*population[mem].fitness/sum;
      }
population[0].cfitness = population[0].rfitness;
/* calculate cumulative fitness */
for (mem = 1; mem < POPSIZE; mem++)
      {
      population[mem].cfitness =  population[mem-1].cfitness +       
                          population[mem].rfitness;
      }
/* finally select survivors using cumulative fitness. */
for (i = 0; i < POPSIZE; i++){ 
      // p = rand()%1000/1000.0;
      r=rand() & 0xff;
	  r|=(rand() & 0xff) << 8;
	  r|=(rand() & 0xff) << 16;
	  r|=(rand() & 0xff) << 24;
      p = (r%100000)/100000.0;
      if (p < population[0].cfitness){
		 copiastruct(&newpopulation[i],&population[0]);
      }else
            {
            for (j = 0; j < POPSIZE;j++)      
                  if (p >= population[j].cfitness && 
                              p<population[j+1].cfitness)
							  copiastruct(&newpopulation[i],&population[j+1]);
                        // newpopulation[i] = population[j+1];
            }
      }

/* once a new population is created, copy it back */
for (i = 0; i < POPSIZE; i++)
      //population[i] = newpopulation[i];      
	  copiastruct(&population[i],&newpopulation[i]);
}


void select(void)
{
int mem,i,j;
double sum = 0;

/* find total fitness of the population */
for (mem = 0; mem < POPSIZE; mem++)
      {
      sum += population[mem].fitness;
      }
/* calculate relative fitness */
for (mem = 0; mem < POPSIZE; mem++)
   population[mem].rfitness =  100.0*population[mem].fitness/sum;

// tomamos dos individuos de la poblacion aleatorios
for(mem=0;mem<POPSIZE;mem++){
    i=randval(0,POPSIZE-1);
    j=randval(0,POPSIZE-2);
	if(j>=i) ++j;
	if(population[i].rfitness>population[j].rfitness)
	   copiastruct(&newpopulation[mem],&population[i]);
	else copiastruct(&newpopulation[mem],&population[j]);
}

/* once a new population is created, copy it back */
for (i = 0; i < POPSIZE; i++)
      //population[i] = newpopulation[i];      
	  copiastruct(&population[i],&newpopulation[i]);
}

/*************************************************************/
/* Swap: A swap procedure that helps in swapping 2 variables */
/*************************************************************/
void swap(unsigned char *x, unsigned char *y)
{
unsigned char temp;
temp = *x;
*x = *y;
*y = temp;
}

/**************************************************************/
/* Crossover: performs crossover of the two selected parents. */
/**************************************************************/
void Xover(int one, int two)
{
   int i;
   int point; 
   double x;

   if(nvars > 1){
      if(nvars == 2)
         point = 1;
      else  // point = (rand() % (nvars - 1)) + 1;
         point = (rand() % (nvars)) + 1;

   // prueba
   // for (i = 0; i < point; i++)
   //   population[one].gene[i]=1+(population[one].gene[i]*population[two].gene[i])%5;
   // one pont crossover
   //   for(i=0;i<point;i++)
   //	    swap(&population[one].gene[i],&population[two].gene[i]);
// uniform crossover
      for(i=0;i<nvars;i++){
         x = rand()%1000/1000.0;
         if(x<0.5)
	        swap(&population[one].gene[i],&population[two].gene[i]);
	  }
   }
}


/***************************************************************/
/* Crossover selection: selects two parents that take part in  */
/* the crossover. Implements a single point crossover          */
/***************************************************************/
void crossover(void)
{
int mem, one;
int first  =  0; /* count of the number of members chosen */
double x;

for (mem = 0; mem < POPSIZE; ++mem){
      x = rand()%1000/1000.0;
      if (x < PXOVER)
            {
            ++first;
            if (first % 2 == 0)
                  Xover(one, mem);
            else
                  one = mem;
            }
      }
}

/**************************************************************/
/* Mutation: Random uniform mutation. A variable selected for */
/* mutation is replaced by a random value between lower and   */ 
/* upper bounds of this variable                              */
/**************************************************************/ 
void mutate(void)
{
int i, j;
unsigned char lbound, hbound;
double x;
for (i = 0; i < POPSIZE; i++)
      for (j = 0; j < nvars; j++)
            {
            x = rand()%1000/1000.0;
            if (x < PMUTATION)
                  {
                  /* find the bounds on the variable to be mutated */
                  lbound = population[i].lower;
                  hbound = population[i].upper;  
                  population[i].gene[j] = (unsigned char) randval(1,5);
                  }
	  }  
}


void reglas(void)
{
   int i,j;
/*   for (i = 0; i < POPSIZE; i++)
      for (j = 2; j < nvars; j++)
         while(population[i].gene[j]==population[i].gene[j-1] && 
		    population[i].gene[j-1]==population[i].gene[j-2])
	        population[i].gene[j]=(unsigned char) randval(1,5);
*/
   for (i = 0; i < POPSIZE; i++)
      for (j = 1; j < nvars; j++)
         while(population[i].gene[j]==population[i].gene[j-1])
	        population[i].gene[j]=(unsigned char) randval(1,5);

}

/***************************************************************/
/* Report function: Reports progress of the simulation. Data   */
/* dumped into the  output file are separated by commas        */
/***************************************************************/

void report(void)
{
int i;
double best_val;            /* best population fitness */
double avg;                 /* avg population fitness */
double stddev;              /* std. deviation of population fitness */
double sum_square;          /* sum of square for std. calc */
double square_sum;          /* square of sum for std. calc */
double sum;                 /* total population fitness */
sum = 0.0;
sum_square = 0.0;
for (i = 0; i < POPSIZE; i++)
      {
      sum += population[i].fitness;
      sum_square += population[i].fitness * population[i].fitness;
      }
avg = sum/(double)POPSIZE;
square_sum = avg * avg * POPSIZE;
stddev = sqrt((sum_square - square_sum)/(POPSIZE - 1));
best_val = population[POPSIZE].fitness;
fprintf(galog,"\n%5d,      %6.3f, %6.3f, %6.3f \n \n", generation, best_val, avg, stddev);
}

/**************************************************************/
/* Main function: Each generation involves selecting the best */
/* members, performing crossover & mutation and then          */
/* evaluating the resulting population, until the terminating */
/* condition is satisfied                                     */
/**************************************************************/
void genetic(MELODIA *melodia,int penta)
{
   int i,c,v,numnotas,count,nfin;
   double bestf;
   
//   if((galog = fopen("galog.txt","w"))==NULL){
//      exit(1);
//   }
   generation = 0;
//   fprintf(galog, "\n generation  best  average  standard \n");
//   fprintf(galog, " number      value fitness  deviation \n");
   initialize(melodia,penta);
   evaluate(melodia,penta);
   keep_the_best();
   while(generation<MAXGEN){
      generation++;
      select();
      crossover();
      mutate();
	  reglas();
//      report();
      evaluate(melodia,penta);
      elitist();
	  bestf=population[POPSIZE].fitness;
   }
   delete [] buf;
//   fprintf(galog,"\n\n Simulation completed\n"); 
//   fprintf(galog,"\n Best member: \n"); 
//   for (i = 0; i < nvars; i++){
//      fprintf (galog,"\n var(%d) = %d \n",i,(int)population[POPSIZE].gene[i]);
//   }
//   fprintf(galog,"\n\n Best fitness = %3.3f\n",population[POPSIZE].fitness);
//   fclose(galog);
//   printf("Success\n");
   for(c=count=0;c<melodia->numcompases;c++){
      numnotas=melodia->penta[penta].compas[c].numnotas;
	  for(v=0;v<numnotas;v++){
		 if(melodia->penta[penta].compas[c].note[v]==0){
            melodia->penta[penta].compas[c].fingering[v]=0;  // si silencio no fingering
			continue;
		 }
		 nfin=population[POPSIZE].gene[count++];
	     if(penta==0) 
			 melodia->penta[penta].compas[c].fingering[v]=nfin;
		 else 
			 melodia->penta[penta].compas[c].fingering[v]=6-nfin;
	  }
   }

   for (i = 0; i < POPSIZE+1; i++){
      delete [] population[i].gene;
      delete [] newpopulation[i].gene;
   }
   return;
}


/*
int calccosto(int fo,int fd,int dist)
{
   int f,res;
   unsigned char c11[]={0,1,1,2,2,10,10,10,10,10,10,10,10,10,10,10};
   
   unsigned char c12p[]={0,0,0,1,2,3,7,10,10,10,10,10,10,10,10,10};
   unsigned char c12n[]={0,0,0,10,10,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c21p[]={0,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c21n[]={0,0,0,1,2,3,7,10,10,10,10,10,10,10,10,10};
   
   unsigned char c13p[]={0,2,1,0,1,2,3,8,10,10,10,10,10,10,10,10};
   unsigned char c13n[]={0,0,0,10,10,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c31p[]={0,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c31n[]={0,2,1,0,1,2,3,8,10,10,10,10,10,10,10,10};

   unsigned char c14p[]={0,4,3,2,1,0,1,2,3,10,10,10,10,10,10,10};
   unsigned char c14n[]={0,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c41p[]={0,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c41n[]={0,4,3,2,1,0,1,2,3,10,10,10,10,10,10,10};

   unsigned char c15p[]={0,8,5,4,3,2,1,0,1,2,3,4,5,10,10,10};
   unsigned char c15n[]={0,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c51p[]={0,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c51n[]={0,8,5,4,3,2,1,0,1,2,3,4,5,10,10,10};

   unsigned char c23p[]={0,1,0,9,10,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c23n[]={0,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c32p[]={0,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c32n[]={0,1,0,9,10,10,10,10,10,10,10,10,10,10,10,10};

   unsigned char c24p[]={0,8,2,1,0,1,2,10,10,10,10,10,10,10,10,10};
   unsigned char c24n[]={0,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20};
   unsigned char c42p[]={0,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c42n[]={0,8,2,1,0,1,2,10,10,10,10,10,10,10,10,10};

   unsigned char c25p[]={0,10,8,2,1,0,1,2,10,10,10,10,10,10,10,10};
   unsigned char c25n[]={0,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c52p[]={0,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c52n[]={0,10,8,2,1,0,1,2,10,10,10,10,10,10,10,10};

   unsigned char c34p[]={0,1,0,9,10,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c34n[]={0,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c43p[]={0,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c43n[]={0,1,0,9,10,10,10,10,10,10,10,10,10,10,10,10};

   unsigned char c35p[]={0,8,2,1,0,1,2,10,10,10,10,10,10,10,10,10};
   unsigned char c35n[]={0,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c53p[]={0,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c53n[]={0,8,2,1,0,1,2,10,10,10,10,10,10,10,10,10};

   unsigned char c45p[]={0,1,0,9,10,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c45n[]={0,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c54p[]={0,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c54n[]={0,1,0,9,10,10,10,10,10,10,10,10,10,10,10,10};

   f=10*fo+fd;
   switch(f){
      case 11:
	  case 22:
      case 33:
	  case 44:
	  case 55:
	     dist=abs(dist);
         res=c11[dist];
		 break;
      case 12:
         if(dist>=0) res=c12p[dist];
		 else res=c12n[-dist];
		 break;
	  case 21:
         if(dist>=0) res=c21p[dist];
		 else res=c21n[-dist];
		 break;
      case 13:
         if(dist>=0) res=c13p[dist];
		 else res=c13n[-dist];
		 break;
	  case 31:
         if(dist>=0) res=c31p[dist];
		 else res=c31n[-dist];
		 break;
      case 14:
         if(dist>=0) res=c14p[dist];
		 else res=c14n[-dist];
		 break;
	  case 41:
         if(dist>=0) res=c41p[dist];
		 else res=c41n[-dist];
		 break;
      case 15:
         if(dist>=0) res=c15p[dist];
		 else res=c15n[-dist];
		 break;
	  case 51:
         if(dist>=0) res=c51p[dist];
		 else res=c51n[-dist];
		 break;
      case 23:
         if(dist>=0) res=c23p[dist];
		 else res=c23n[-dist];
		 break;
	  case 32:
         if(dist>=0) res=c32p[dist];
		 else res=c32n[-dist];
		 break;
      case 24:
         if(dist>=0) res=c24p[dist];
		 else res=c24n[-dist];
		 break;
	  case 42:
         if(dist>=0) res=c42p[dist];
		 else res=c42n[-dist];
		 break;
      case 25:
         if(dist>=0) res=c25p[dist];
		 else res=c25n[-dist];
		 break;
	  case 52:
         if(dist>=0) res=c52p[dist];
		 else res=c52n[-dist];
		 break;
      case 34:
         if(dist>=0) res=c34p[dist];
		 else res=c34n[-dist];
		 break;
	  case 43:
         if(dist>=0) res=c43p[dist];
		 else res=c43n[-dist];
		 break;
      case 35:
         if(dist>=0) res=c35p[dist];
		 else res=c35n[-dist];
		 break;
	  case 53:
         if(dist>=0) res=c53p[dist];
		 else res=c53n[-dist];
		 break;
      case 45:
         if(dist>=0) res=c45p[dist];
		 else res=c45n[-dist];
		 break;
	  case 54:
         if(dist>=0) res=c54p[dist];
		 else res=c54n[-dist];
		 break;
	  default:
		 return -1;
   }
   res=20-res;
   return res;
}
*/

/*int calccosto(int fo,int fd,int dist)
{
   int f,res;
   
   unsigned char c11[]={0,1,1,30,30,30,30,30,30,30,30,30,30,30,30,30};
   
   unsigned char c12p[]={0,0,0,1,4,8,9,30,30,30,30,30,30,30,30,30};
   unsigned char c12n[]={0,0,0,30,30,30,30,30,30,30,30,30,30,30,30,30};
   unsigned char c21p[]={0,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30};
   unsigned char c21n[]={0,0,0,1,4,8,9,30,30,30,30,30,30,30,30,30};
   
   unsigned char c13p[]={0,4,1,0,0,1,4,8,30,30,30,30,30,30,30,30};
   unsigned char c13n[]={0,0,0,10,10,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c31p[]={0,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30};
   unsigned char c31n[]={0,4,1,0,0,1,4,8,30,30,30,30,30,30,30,30};

   unsigned char c14p[]={0,9,8,4,1,0,0,1,4,30,30,30,30,30,30,30};
   unsigned char c14n[]={0,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c41p[]={0,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30};
   unsigned char c41n[]={0,9,8,4,1,0,0,1,4,30,30,30,30,30,30,30};

   unsigned char c15p[]={0,9,9,8,4,1,0,0,1,4,8,9,9,30,30,30};
   unsigned char c15n[]={0,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c51p[]={0,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30};
   unsigned char c51n[]={0,9,9,8,4,1,0,0,1,4,8,9,9,30,30,30};

   unsigned char c23p[]={0,0,0,30,30,30,30,30,30,30,30,30,30,30,30,30};
   unsigned char c23n[]={0,30,30,10,10,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c32p[]={0,30,30,30,30,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c32n[]={0,0,0,30,30,30,30,30,30,30,30,30,30,30,30,30};

   unsigned char c24p[]={0,4,1,0,0,1,4,30,30,30,30,30,30,30,30,30};
   unsigned char c24n[]={0,30,30,30,10,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c42p[]={0,30,30,10,10,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c42n[]={0,4,1,0,0,1,4,30,30,30,30,30,30,30,30,30};

   unsigned char c25p[]={0,10,8,4,1,0,0,1,30,30,30,30,30,30,30,30};
   unsigned char c25n[]={0,30,30,30,30,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c52p[]={0,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c52n[]={0,10,8,4,1,0,0,1,30,30,30,30,30,30,30,30};

   unsigned char c34p[]={0,0,0,30,30,30,30,30,30,30,30,30,30,30,30,30};
   unsigned char c34n[]={0,30,30,30,30,30,30,30,10,10,10,10,10,10,10,10};
   unsigned char c43p[]={0,30,30,10,10,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c43n[]={0,0,0,30,30,30,30,30,30,30,30,30,30,30,30,30};

   unsigned char c35p[]={0,30,1,0,0,1,4,30,30,30,30,30,30,30,30,30};
   unsigned char c35n[]={0,30,30,30,30,30,30,30,10,10,10,10,10,10,10,10};
   unsigned char c53p[]={0,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c53n[]={0,30,1,0,0,1,4,30,30,30,30,30,30,30,30,30};
   
   unsigned char c45p[]={0,0,0,30,30,30,30,30,30,30,30,30,30,30,30,30};
   unsigned char c45n[]={0,30,30,30,30,30,30,30,30,30,10,10,10,10,10,10};
   unsigned char c54p[]={0,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10};
   unsigned char c54n[]={0,0,0,30,30,30,30,30,30,30,30,30,30,30,30,30};
*/

/*
double diversidad(void)
{
   int i,inc,k,clase[64];
   double fit,minf,maxf,n,entropia,p;

   n=POPSIZE;
   for(i=0;i<n;i++) clase[i]=0;
   for(i=0;i<POPSIZE;i++){
      fit=population[i].fitness*100; 
      if(i==0) minf=maxf=fit;
	  if(fit<=minf) minf=fit;
	  if(fit>maxf) maxf=fit;
   }
   inc=(maxf-minf)/n;
   for(i=0;i<POPSIZE;i++){
      fit=population[i].fitness*100; 
	  k=(fit-minf)/inc;
      ++clase[k];
   }
   for(i=entropia=0;i<n;i++){
      p=clase[i]*1.0/POPSIZE;
	  if(p>0) p=p*log(p);
	  entropia+=p;
   }
   entropia*=-1;
   diver=entropia/log(n);
   pc1=PXOVERMIN+(PXOVERMAX-PXOVERMIN)*0.5*diver;
   pc2=PXOVERMAX-(PXOVERMAX-PXOVERMIN)*0.5*(1-diver);
   pm1=PMUTATIONMIN+(PMUTATIONMAX-PMUTATIONMIN)*0.5*(1-diver);
   pm2=PMUTATIONMAX-(PMUTATIONMAX-PMUTATIONMIN)*0.5*diver;
   return diver;
}



void mutate1(void)
{
int i, j,mem;
unsigned char lbound, hbound;
double x;
double fmax,fmin,favg,fp,fit;

for (mem=favg=0;mem<POPSIZE;++mem){
   fit=population[mem].fitness;
   if(mem==0) fmax=fmin=fit;
   if(fit>=fmax) fmax=population[mem].fitness;
   if(fit<fmin) fmin=population[mem].fitness;
   favg+=fit;
}

favg/=POPSIZE;
pmutation=(1-diver)*0.5/(1+diver);
for (i = 0; i < POPSIZE; i++)
      for (j = 0; j < nvars; j++){
            x = rand()%1000/1000.0;
            if (x < pmutation){
               lbound = population[i].lower;
               hbound = population[i].upper;  
               population[i].gene[j] = (unsigned char) randval(1,5);
               fp=population[i].fitness;
               if(fp<favg)
			      pmutation=(pm1*(favg-fp)+pm2*(fp-fmin))/(favg-fmin);
			   if(fp>=favg)
				  pmutation=(pm2*(fp-favg)+0.01*(fmax-fp))/(fmax-favg);			   		 

            }
      }
}

void crossover1(void)
{
int mem, one;
int first  =  0; 
double x;

double fmax,fmin,favg,fp,fit;
for (mem=favg= 0; mem < POPSIZE; ++mem){
   fit=population[mem].fitness;
   if(mem==0) fmax=fmin=fit;
   if(fit>=fmax) fmax=population[mem].fitness;
   if(fit<fmin) fmin=population[mem].fitness;
   favg+=fit;
}
favg/=POPSIZE;

pxover=1.0*diver/(1+diver);
for (mem = 0; mem < POPSIZE; ++mem){
      x = rand()%1000/1000.0;
      if (x < pxover){
         ++first;
         if (first % 2 == 0){
			 Xover(one, mem);
             fp=max(population[one].fitness,population[mem].fitness);
             if(fp<favg){
			    pxover=(pc1*(favg-fp)+pc2*(fp-fmin))/(favg-fmin);
			 }
			 if(fp>=favg)
				pxover=(pc2*(fp-favg)+0.01*(fmax-fp))/(fmax-favg);

         }else
            one = mem;
         }
      }
}


*/