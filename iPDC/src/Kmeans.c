/*Pavan Changes*/

#include "parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <math.h>

FILE *fp;
/*intial weights  weights for each points */
unsigned long long int count_A = 1000;
unsigned long long int count_B = 1000;
unsigned long long int count_C = 1000;

/*In India frequency ranges from 49.5 to 50.2 Hz*/
long double std_dev = 0.6454972244;
long double A = 0;             // for point A=50
long double B = -1.5491933394; // for point B=49
long double C = 0.7745966692;  // for point C=50.5
long double mean = 50;
unsigned long long int count = 3000;

gboolean kmeans(struct data_frame *df)
{
    long double CURR_FREQ = 50 + to_intconvertor(df->dpmu[0]->freq) * 1e-3;
    long double CURR_FREQ_temp = CURR_FREQ;
    CURR_FREQ = (CURR_FREQ - mean) / std_dev;
    long double diff_A = fabs(A - CURR_FREQ);
    long double diff_B = fabs(B - CURR_FREQ);
    long double diff_C = fabs(C - CURR_FREQ);
    // fp = fopen("points.csv","a");
    // printf("A: %Lf, B: %Lf, C: %Lf\n",A,B,C);
    // fprintf(fp,"%Lf, %Lf, %Lf\n",A,B,C);
    // fclose(fp);

    /*Updating std_dev and mean*/
    // printf("std_dev: %Lf, mean: %Lf\n",std_dev,mean);
    mean = ((mean * count) + CURR_FREQ_temp) / (++count);
    std_dev = (((std_dev * std_dev) * (count - 1)) + (CURR_FREQ * std_dev * CURR_FREQ * std_dev)) / count;
    std_dev = sqrt(std_dev);

    if (diff_A <= diff_B && diff_A <= diff_C)
    {
        A = ((count_A * (A)) + CURR_FREQ) / (++count_A);
        return TRUE;
    }
    else if (diff_B < diff_C)
    {
        B = ((count_B * (B)) + CURR_FREQ) / (++count_B);
        return FALSE;
    }
    else
    {
        C = ((count_C * (C)) + CURR_FREQ) / (++count_C);
        return FALSE;
    }
}

/*Pavan Changes*/