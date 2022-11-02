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

struct Kmeans2
{
    int idcode;
    int count;
    struct Point *P;
    struct Kmeans2 *next;
};

struct Kmeans2 *head_of_kmeans2 = NULL;

long double distance(struct Point *A, struct Point *B)
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

bool Kmeans2(struct data_frame *df)
{
    if (head_of_kmeans2 == NULL)
    {
        head_of_kmeans2 = (struct Kmeans2 *)malloc(sizeof(struct Kmeans2));
        head_of_kmeans2->idcode = to_intconvertor(df->idcode);
        head_of_kmeans2->count = 0;
        head_of_kmeans2->next = NULL;
        head_of_kmeans2->P = NULL;
        return true;
    }
    else
    {
        struct Kmeans2 *temp = head_of_kmeans2;
        struct Kmeans2 *previous = NULL;
        while (temp != NULL)
        {
            if (temp->idcode == to_intconvertor(df->idcode))
            {
                printf("count: %d\n",temp->count);
                if (temp->count == 0)
                {
                    temp->P = (struct Point *)malloc(sizeof(struct Point) * 500);
                }
                if (temp->count != 500)
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
                    temp->P[temp->count].x = CURR_FREQ;
                    temp->P[temp->count].y = CURR_vol;
                    temp->P[temp->count].cluster = -1;
                    temp->P[temp->count].minDist = __DBL_MAX__;
                    temp->count++;
                }
                else
                {
                    int no_of_clusters = 5;
                    int epochs = 20;
                    int *c = getRandoms(0, 499, no_of_clusters);
                    struct Point *Centroids = (struct Point *)malloc(sizeof(struct Point) * no_of_clusters);
                    for (int i = 0; i < no_of_clusters; i++)
                    {
                        Centroids[i].x = temp->P[c[i]].x;
                        Centroids[i].y = temp->P[c[i]].y;
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
                                long double dist = distance(&Centroids[i], &temp->P[j]);
                                if (temp->P[j].minDist > dist)
                                {
                                    temp->P[j].minDist = dist;
                                    temp->P[j].cluster = i;
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
                            nPoints[temp->P[i].cluster]++;
                            Sumx[temp->P[i].cluster] += temp->P[i].x;
                            Sumy[temp->P[i].cluster] += temp->P[i].y;
                            temp->P[i].minDist = __DBL_MAX__;
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
                    temp->count = 0;
                    FILE *fp;
                    fp = fopen("kmeans.txt", "a");

                    for (int i = 0; i < 500; i++)
                    {
                        fprintf(fp, "%Lf, %Lf, %d\n", temp->P[i].x, temp->P[i].y, temp->P[i].cluster);
                    }
                    fprintf(fp, "\n\n");

                    for (int i = 0; i < no_of_clusters; i++)
                    {
                        fprintf(fp, "%d : %Lf, %Lf\n", i, Centroids[i].x, Centroids[i].y);
                    }

                    fprintf(fp, "\n\n");

                    fclose(fp);

                    free(Centroids);
                    free(temp->P);
                }
                return true;
                break;
            }
            previous = temp;
            temp = temp->next;
        }
        if (temp == NULL)
        {
            struct Kmeans2 *bring = (struct Kmeans2 *)malloc(sizeof(struct Kmeans2));
            bring->idcode = to_intconvertor(df->idcode);
            bring->count = 0;
            bring->next = NULL;
            bring->P = NULL;
            previous->next = bring;
            return true;
        }
    }
}