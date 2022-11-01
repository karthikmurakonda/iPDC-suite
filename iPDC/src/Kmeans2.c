#include "parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

struct Point
{
    long double x, y;    // coordinates
    int cluster;         // no default cluster
    long double minDist; // default infinite dist to nearest cluster
};

int count = 0;
struct Point *P;

long double distance(struct Point* A, struct Point* B)
{
    return (((A->x - B->x) * (A->x - B->x)) + ((A->y - B->y) * (A->y - B->y)));
}

int *getRandoms(int lower, int upper, int count)
{
    srand(time(0));
    int *p = (int *)malloc(sizeof(int) * count);
    int track = 0;
    while (1)
    {
        if (track == count)
        {
            return p;
        }
        int num = (rand() % (upper - lower + 1)) + lower;
        if (track == 0)
        {
            p[track] = num;
            track++;
            continue;
        }
        int flag = 0;
        for (int j = 0; j <= track - 1; j++)
        {
            if (num == p[j])
            {
                flag = 1;
                break;
            }
        }
        if (flag == 0)
        {
            p[track] = num;
            track++;
        }
    }
}

void Kmeans2(struct data_frame *df)
{
    printf("count: %d\n",count);
    if(count==0)
    {
        P = (struct Point *)malloc(sizeof(struct Point) * 500);
    }
    if (count != 500)
    {
        float CURR_FREQ;
        if (df->dpmu[0]->fmt->freq == '0')
        {
            CURR_FREQ = 50 + to_intconvertor(df->dpmu[0]->freq) * 1e-3;
        }
        else
        {
            CURR_FREQ = decode_ieee_single(df->dpmu[0]->freq);
        }
        float CURR_vol;
        if (df->dpmu[0]->fmt->phasor == '0')
        {
            if (df->dpmu[0]->fmt->polar == '0')
            {
                unsigned char s1[2];
                unsigned char s2[2];
                strncpy(s1, df->dpmu[0]->phasors[0], 2);
                strncpy(s2, df->dpmu[0]->phasors[0] + 2, 2);
                long double v1 = to_intconvertor(s1);
                long double v2 = to_intconvertor(s2);
                CURR_vol = sqrt((v1 * v1) + (v2 * v2));
            }
            else
            {
                unsigned char s1[2];
                strncpy(s1, df->dpmu[0]->phasors[0], 2);
                CURR_vol = to_intconvertor(s1);
            }
        }
        else
        {
            if (df->dpmu[0]->fmt->polar == '0')
            {
                unsigned char s1[4];
                unsigned char s2[4];
                strncpy(s1, df->dpmu[0]->phasors[0], 4);
                strncpy(s2, df->dpmu[0]->phasors[0] + 2, 4);
                long double v1 = decode_ieee_single(s1);
                long double v2 = decode_ieee_single(s2);
                CURR_vol = sqrt((v1 * v1) + (v2 * v2));
            }
            else
            {
                unsigned char s1[4];
                strncpy(s1, df->dpmu[0]->phasors[0], 4);
                CURR_vol = decode_ieee_single(s1);
            }
        }
        P[count].x = CURR_FREQ;
        P[count].y = CURR_vol;
        P[count].cluster = -1;
        P[count].minDist = __DBL_MAX__;
        count++;
    }
    else
    {
        int no_of_clusters = 5;
        int epochs = 20;
        int *c = getRandoms(0, 499, no_of_clusters);
        struct Point *Centroids = (struct Point *)malloc(sizeof(struct Point) * no_of_clusters);
        for (int i = 0; i < no_of_clusters; i++)
        {
            Centroids[i].x = P[c[i]].x;
            Centroids[i].y = P[c[i]].y;
            Centroids[i].minDist = __DBL_MAX__;
            Centroids[i].cluster = -1;
        }

        free(c);

        while (epochs--)
        {
            for (int i = 0; i < no_of_clusters; i++)
            {
                for (int j = 0; j < 500; j++)
                {
                    long double dist = distance(&Centroids[i], &P[j]);
                    if (P[j].minDist > dist)
                    {
                        P[j].minDist = dist;
                        P[j].cluster = i;
                    }
                }
            }

            int *nPoints = (int *)malloc(sizeof(int) * no_of_clusters);
            long double *Sumx = (long double *)malloc(sizeof(long double) * no_of_clusters);
            long double *Sumy = (long double *)malloc(sizeof(long double) * no_of_clusters);

            for (int i = 0; i < no_of_clusters; i++)
            {
                nPoints[i] = 0;
                Sumx[i] = 0;
                Sumy[i] = 0;
            }

            for (int i = 0; i < 500; i++)
            {
                nPoints[P[i].cluster]++;
                Sumx[P[i].cluster] += P[i].x;
                Sumy[P[i].cluster] += P[i].y;
                P[i].minDist = __DBL_MAX__;
            }

            for (int i = 0; i < no_of_clusters; i++)
            {
                Centroids[i].x = Sumx[i] / nPoints[i];
                Centroids[i].y = Sumy[i] / nPoints[i];
            }

            free(nPoints);
            free(Sumx);
            free(Sumy);
        }
        count = 0;
        FILE *fp;
        fp = fopen("kmeans.txt","a");
        
        for(int i=0;i<500;i++)
        {
            fprintf(fp,"%Lf, %Lf, %d\n",P[i].x,P[i].y,P[i].cluster);
        }
        fprintf(fp,"\n\n");

        for(int i=0;i<no_of_clusters;i++)
        {
            fprintf(fp,"%d : %Lf, %Lf\n",i,Centroids[i].x,Centroids[i].y);
        }

        fprintf(fp,"\n\n");

        fclose(fp);

        free(Centroids);
        free(P);
    }
}