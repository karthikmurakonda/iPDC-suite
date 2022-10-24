
#include "parser.h"
#include <stdio.h>
#include <math.h>
#include <gtk/gtk.h>
#include <stdlib.h>

long double *freq1;
long double *freq2;
int count_track1=0;
int count_track2=0;
int flag=0;

void DTWdistance(struct data_frame *df,gboolean *result)
{
    float CURR_FREQ = 50 + to_intconvertor(df->dpmu[0]->freq)*1e-3;
    //printf("count1: %d\ncount2: %d\n",count_track1,count_track2);
    //printf("curr_freq: %f\n",CURR_FREQ);
    if(count_track1==0)
    {
        if(flag==0)
        {
            flag=1;
            freq1 = (long double *)malloc(500 * sizeof(long double));
        }
        freq1[count_track2]=CURR_FREQ;
        count_track2++;
        if(count_track2==500)
        {
            count_track2=0;
            flag=0;
            count_track1++;
        }
    }
    else if(count_track1==1)
    {
        if(flag==0)
        {
            flag=1;
            freq2 = (long double *)malloc(500 * sizeof(long double));
        }
        freq2[count_track2]=CURR_FREQ;
        count_track2++;
        if(count_track2==500)
        {
            count_track2=0;
            flag=0;
            count_track1++;
        }
    }
    else
    {
        float** DTW = (float**)malloc(501 * sizeof(float*));
        for (int i = 0; i < 501; i++)
            DTW[i] = (float*)malloc(501 * sizeof(float));

        for(int i=0;i<501;i++)
        {
            for(int j=0;j<501;j++)
                DTW[i][j]=1000;
        }
        DTW[0][0]=0;

        for(int i=1;i<501;i++)
        {
            for(int j=1;j<501;j++)
            {
                float cost=fabs(freq1[i-1]-freq2[j-1]);
                //printf("cost: %f",cost);
                DTW[i][j]=cost+fmin(DTW[i][j-1],fmin(DTW[i-1][j],DTW[i-1][j-1]));
                //printf(" DTW[%d][%d]: %f\n",i,j,DTW[i][j]);
            }
        }

        printf("DTWdistance: %f\n",DTW[500][500]);
        if(DTW[500][500]>=35) *result=false;
        else *result=true;
        free(freq1);
        free(freq2);
        free(DTW);
        count_track1=0;
    }
}