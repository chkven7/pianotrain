/* Design Document : Comparision of two Speech Signals by DTW algorithm :

Step 1 : Generate a text file ( Like signal1.txt, signal2.txt ) of each speech signal by some softare or program.

Step 2 : Generate frames (Each frame contain a set of cepstral coefficients) by splitting the voiced region from that text file
		 and save these frames in order in a text file( Like input1.txt, input2.txt ).
		 where each row represent a frame and contains frame's set of coefficients as column elements.

Step 3 : Apply this program "dtw.c" only after doing step1 and step2 to find similerities and differences between signal1 and signal2.
		 It take two text files as input ( Like "input1.txt" and "input2.txt" ) and give Optimal Warping Path and Cost as output.
*/

/* Input format :-  input.txt : 9 frames of speech signal and each frame has 12 cepstral coefficient
1	1	1	1	1	1	1	1	1	1	1	1
2	2	2	2	2	2	2	2	2	2	2	2
3	3	3	3	3	3	3	3	3	3	3	3
4	4	4	4	4	4	4	4	4	4	4	4
5	5	5	5	5	5	5	5	5	5	5	5
6	6	6	6	6	6	6	6	6	6	6	6
7	7	7	7	7	7	7	7	7	7	7	7
8	8	8	8	8	8	8	8	8	8	8	8
9	9	9	9	9	9	9	9	9	9	9	9

*/

// Assumption : All cepstral coefficients are integer type.

#include <stdio.h>
#include <math.h>

double distance_measure(int *x,int *y,int len)
{
	int i;
    double xx,xy,yy;
	double ppmcc;
	double x_mean,y_mean;

	xx = 0;
    xy = 0;
    yy = 0;
	x_mean=y_mean=0;
    for(i=0; i<len; ++i ){
       x_mean+=x[i];
	   y_mean+=y[i];
	}
	x_mean/=len; y_mean/=len;
    for(i=0; i<len; ++i ){
       xx += (x[i]-x_mean)*(x[i]-x_mean);
       xy += (x[i]-x_mean)*(y[i]-y_mean);
       yy += (y[i]-y_mean)*(y[i]-y_mean);
	}
    ppmcc = xy/sqrt(xx*yy);
	return 1-ppmcc;
}

double distance_measure1(int *x,int *y,int len)
{
   int k;
   double total;

   total = 0;
   for (k=0;k<len;k++)
      total = total + ((x[k] - y[k]) * (x[k] - y[k]));
   return total;
}

double distance_measure2(int *x,int *y,int len)
{
   int k;
   double total;

   total = 0;
   for (k=0;k<len;k++)
      total = total + abs(x[k] - y[k]);
   return total;
}
double min( double x, double y, double z ) // return minimum among integer x, y and z
{
    if( ( x <= y ) && ( x <= z ) ) return x;
    if( ( y <= x ) && ( y <= z ) ) return y;
    if( ( z <= x ) && ( z <= y ) ) return z;
}

double dtw2(int **x,int **y,unsigned int xsize,unsigned int ysize,unsigned int params)
{
   int i,k,n,m;
   double tempr;

   n=xsize;
   m=ysize;

   double **local_distance;

   local_distance=(double **) new double*[n];
   for (i=0; i < n; i++) local_distance[i]=(double *) new double[m];
   for(i=0; i<n; i++){
      for(k=0; k<m; k++){
        local_distance[i][k] = distance_measure2(x[i],y[k],params);
	  }
   }

   double **global_distance;
   global_distance=(double **) new double*[n];
   for (i=0; i < n; i++) global_distance[i]=(double *) new double[m];

   global_distance[0][0] = local_distance[0][0];
   for(i=1; i<n; i++) // generating first row element of global_distance matrix
      global_distance[i][0] = local_distance[i][0] + global_distance[i-1][0];

   for(k=1; k<m; k++) // generating first column element of global_distance matrix{
        global_distance[0][k] = local_distance[0][k] + global_distance[0][k-1];
   for(i=1; i<n; i++){
      for(k=1; k<m; k++){
        global_distance[i][k] = local_distance[i][k] + min(global_distance[i-1][k],global_distance[i-1][k-1],global_distance[i][k-1]);
	  }
   }

   i=0;
   k=0;
   while((i!=n-1)||(k!=m-1)){
      if(i==n-1) // if you reached to the last row (n-1) then go only one step forward in last row
        k=k+1;
      else if(k==m-1) //if you have reached to the last column (m-1) then go only one step upward in last column
        i=i+1;
      else{   
		double global_minm = min(global_distance[i+1][k],global_distance[i+1][k+1],global_distance[i][k+1]);

        if(global_distance[i+1][k] == global_minm){
            i=i+1;
        }
        else if(global_distance[i+1][k+1] == global_minm)
        {
            i=i+1;
            k=k+1;
        }
        else//(global_distance[i][k+1] == global_minm)
        {
            k=k+1;
        }
	  }
   }
   tempr=global_distance[n-1][m-1];
   for (i=0; i < n; i++) delete [] local_distance[i];
   for (i=0; i < n; i++) delete [] global_distance[i];
   delete [] local_distance;
   delete [] global_distance;
   return tempr;
}
