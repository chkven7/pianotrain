#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>
#include "piano.h"

static void initialize(MELODIA *melodia,int penta,unsigned char *&buf,int &count)
{
   int j,c,numcompas,numnotas,v;

   count=0;
   numcompas=melodia->numcompases;
   for(j=0;j<numcompas;j++)
      count+=(melodia->penta[penta].compas[j].numnotas-melodia->penta[penta].compas[j].numsilencios);                 
   if(count==0){ // todo silencios
      buf=NULL;
	  return;
   }
   buf=(unsigned char *) new unsigned char[count];
   for(c=count=0;c<melodia->numcompases;c++){
      numnotas=melodia->penta[penta].compas[c].numnotas;
      for(v=0;v<numnotas;v++){		 
		  if(melodia->penta[penta].compas[c].note[v]==0) continue;   // descarta silencios
          buf[count++]=melodia->penta[penta].compas[c].notekey[v];
	  }
   }  
   return;
}

static int calccosto(int fo,int fd,int dist)
{
   int f,res;
   
   unsigned char c11[]={0,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30};
   
   unsigned char c12p[]={0,0,0,1,4,8,9,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29};
   unsigned char c12n[]={0,0,0,10,10,10,10,10,10,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30};
   unsigned char c21p[]={0,10,10,10,10,10,10,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30};
   unsigned char c21n[]={0,0,0,1,4,8,9,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29};
   
   unsigned char c13p[]={0,4,1,0,0,1,4,8,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29};
   unsigned char c13n[]={0,0,0,0,10,10,10,10,10,10,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30};
   unsigned char c31p[]={0,10,10,10,10,10,10,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30};
   unsigned char c31n[]={0,4,1,0,0,1,4,8,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29};

   unsigned char c14p[]={0,9,8,4,1,0,0,1,4,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29};
   unsigned char c14n[]={0,10,10,10,10,10,10,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30};
   unsigned char c41p[]={0,10,10,10,10,10,10,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30};
   unsigned char c41n[]={0,9,8,4,1,0,0,1,4,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29};

   unsigned char c15p[]={0,9,9,8,4,1,0,0,1,4,8,9,9,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29};
   unsigned char c15n[]={0,10,10,10,10,10,10,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30};
   unsigned char c51p[]={0,10,10,10,10,10,10,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30};
   unsigned char c51n[]={0,9,9,8,4,1,0,0,1,4,8,9,9,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29};

   unsigned char c23p[]={0,0,0,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29};
   unsigned char c23n[]={0,30,30,10,10,10,10,10,10,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30};
   unsigned char c32p[]={0,10,10,10,10,10,10,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30};
   unsigned char c32n[]={0,0,0,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29};

   unsigned char c24p[]={0,4,1,0,0,1,4,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29};
   unsigned char c24n[]={0,10,10,10,10,10,10,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30};
   unsigned char c42p[]={0,10,10,10,10,10,10,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30};
   unsigned char c42n[]={0,4,1,0,0,1,4,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29};

   unsigned char c25p[]={0,10,8,4,1,0,0,1,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29};
   unsigned char c25n[]={0,10,10,10,10,10,10,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30};
   unsigned char c52p[]={0,10,10,10,10,10,10,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30};
   unsigned char c52n[]={0,10,8,4,1,0,0,1,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29};

   unsigned char c34p[]={0,0,0,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29};
   unsigned char c34n[]={0,10,10,10,10,10,10,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30};
   unsigned char c43p[]={0,10,10,10,10,10,10,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30};
   unsigned char c43n[]={0,0,0,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29};

   unsigned char c35p[]={0,30,1,0,0,1,4,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29};
   unsigned char c35n[]={0,10,10,10,10,10,10,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30};
   unsigned char c53p[]={0,10,10,10,10,10,10,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30};
   unsigned char c53n[]={0,30,1,0,0,1,4,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29};
   
   unsigned char c45p[]={0,0,0,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29};
   unsigned char c45n[]={0,10,10,10,10,10,10,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30};
   unsigned char c54p[]={0,10,10,10,10,10,10,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30};
   unsigned char c54n[]={0,0,0,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29,29};
   
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

int viterbi(MELODIA *melodia,int penta)
{
   unsigned char *buf;
   int k,l,v,t,vmin,lmin,count,dist,costototal;

   initialize(melodia,penta,buf,count);
   if(count==0){  // no hay notas, es todo silencios
	  int c,numnotas;
      for(c=0;c<melodia->numcompases;c++){
         numnotas=melodia->penta[penta].compas[c].numnotas;
	     for(v=0;v<numnotas;v++){
		    if(melodia->penta[penta].compas[c].note[v]==0){
               melodia->penta[penta].compas[c].fingering[v]=0;  // si silencio no fingering
			   continue;
			}
		 }
	  }
      return -1;
   }
   int **G= new int*[count];
   int **L= new int*[count];
   for(t=0;t<count;t++){
      G[t]=new int[6];
	  L[t]=new int[6];
   }
   for(t=0;t<count;t++)
	  for(k=1;k<=5;k++)
		  if(t==0) G[t][k]=L[t][k]=0; 
   for(t=1;t<count;t++){
	  dist=buf[t]-buf[t-1];
	  for(k=1;k<=5;k++){
		 for(l=1;l<=5;l++){
		    v=G[t-1][l]+calccosto(l,k,dist);
			if(l==1){ vmin=v; lmin=l; }
			if(v<=vmin){ vmin=v; lmin=l; }
		 }
		 G[t][k]=vmin; L[t][k]=lmin;
	  }
   }
   for(k=1;k<=5;k++){
	  v=G[t-1][k];
      if(k==1) vmin=v;
	  if(v<=vmin){ vmin=v; lmin=k; }
   }
   costototal=vmin;
   buf[count-1]=lmin;
   for(t=count-1;t>0;t--){
      lmin=L[t][lmin];
	  buf[t-1]=lmin;
   }
   for(t=0;t<count;t++){
      delete [] G[t];
	  delete [] L[t];
   }
   delete [] G;
   delete [] L;

   int c,numnotas,nfin;
   for(c=count=0;c<melodia->numcompases;c++){
      numnotas=melodia->penta[penta].compas[c].numnotas;
	  for(v=0;v<numnotas;v++){
		 if(melodia->penta[penta].compas[c].note[v]==0){
            melodia->penta[penta].compas[c].fingering[v]=0;  // si silencio no fingering
			continue;
		 }
		 nfin=buf[count++];
	     if(penta==0) 
			 melodia->penta[penta].compas[c].fingering[v]=nfin;
		 else 
			 melodia->penta[penta].compas[c].fingering[v]=6-nfin;
	  }
   }
   delete [] buf;
   return costototal;
}

//************************************
// viterbi para notereading()
//************************************

static void initialize(NOTA *listanotas,int can,int penta,unsigned char *&buf,int &count)
{
   int j;

   count=0;
   for(j=0;j<can;j++){
      if(listanotas[j].penta!=penta) continue;
	  ++count;
   }
   buf=(unsigned char *) new unsigned char[count];
   for(j=count=0;j<can;j++){
      if(listanotas[j].penta!=penta) continue;      
      buf[count++]=listanotas[j].notakey;
   }
   return;
}

int viterbi(NOTA *listanotas,int can,int penta)
{
   unsigned char *buf;
   int k,l,v,t,vmin,lmin,count,dist,costototal;

   initialize(listanotas,can,penta,buf,count);
   int **G= new int*[count];
   int **L= new int*[count];
   for(t=0;t<count;t++){
      G[t]=new int[6];
	  L[t]=new int[6];
   }
   for(t=0;t<count;t++)
	  for(k=1;k<=5;k++)
		  if(t==0) G[t][k]=L[t][k]=0; 
   for(t=1;t<count;t++){
	  dist=buf[t]-buf[t-1];
	  for(k=1;k<=5;k++){
		 for(l=1;l<=5;l++){
		    v=G[t-1][l]+calccosto(l,k,dist);
			if(l==1){ vmin=v; lmin=l; }
			if(v<=vmin){ vmin=v; lmin=l; }
		 }
		 G[t][k]=vmin; L[t][k]=lmin;
	  }
   }
   for(k=1;k<=5;k++){
	  v=G[t-1][k];
      if(k==1) vmin=v;
	  if(v<=vmin){ vmin=v; lmin=k; }
   }
   costototal=vmin;
   buf[count-1]=lmin;
   for(t=count-1;t>0;t--){
      lmin=L[t][lmin];
	  buf[t-1]=lmin;
   }
   for(t=0;t<count;t++){
      delete [] G[t];
	  delete [] L[t];
   }
   delete [] G;
   delete [] L;

   int c,nfin;
   for(c=count=0;c<can;c++){
      if(listanotas[c].penta!=penta) continue;
      nfin=buf[count++];
	  if(penta==0) 
		 listanotas[c].finger=nfin;
	  else 
		 listanotas[c].finger=6-nfin;
   }   
   delete [] buf;
   return costototal;
}

   
