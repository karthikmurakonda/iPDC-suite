#include "parser.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <gtk/gtk.h>
#include <stdlib.h>

typedef struct
{
    int idcode;
    long double *freq1;
    long double *freq2;
    int count_track1;
    int count_track2;
    int flag;
    int result;
    DTWfreqlist *next;
} DTWfreqlist;

typedef struct
{
    int idcode;
    long double *vol1;
    long double *vol2;
    int count_track1;
    int count_track2;
    int flag;
    int result;
    DTWvollist *next;
} DTWvollist;


DTWfreqlist *head = NULL;
DTWvollist *headvol = NULL;

void DTWfreqDistance(struct data_frame *df)
{
    if (head == NULL)
    {
        head = (DTWfreqlist *)malloc(sizeof(DTWfreqlist));
        head->count_track1 = 0;
        head->count_track2 = 0;
        head->flag = 0;
        head->idcode = to_intconvertor(df->idcode);
        head->next = NULL;
        head->result=1;
        return 1;
    }
    else
    {
        DTWfreqlist *temp = head;
        DTWfreqlist *previous;
        while (temp != NULL)
        {
            if (to_intconvertor(df->idcode) == temp->idcode)
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

                // printf("count1: %d\ncount2: %d\n",count_track1,count_track2);
                // printf("curr_freq: %f\n",CURR_FREQ);
                if (temp->count_track1 == 0)
                {
                    if (tmp->flag == 0)
                    {
                        temp->flag = 1;
                        temp->freq1 = (long double *)malloc(500 * sizeof(long double));
                    }
                    temp->freq1[temp->count_track2] = CURR_FREQ;
                    temp->count_track2++;
                    if (temp->count_track2 == 500)
                    {
                        temp->count_track2 = 0;
                        temp->flag = 0;
                        temp->count_track1++;
                    }
                }
                else if (temp->count_track1 == 1)
                {
                    if (temp->flag == 0)
                    {
                        temp->flag = 1;
                        temp->freq2 = (long double *)malloc(500 * sizeof(long double));
                    }
                    temp->freq2[temp->count_track2] = CURR_FREQ;
                    temp->count_track2++;
                    if (temp->count_track2 == 500)
                    {
                        temp->count_track2 = 0;
                        temp->flag = 0;
                        temp->count_track1++;
                    }
                }
                else
                {
                    float **DTW = (float **)malloc(501 * sizeof(float *));
                    for (int i = 0; i < 501; i++)
                        DTW[i] = (float *)malloc(501 * sizeof(float));

                    for (int i = 0; i < 501; i++)
                    {
                        for (int j = 0; j < 501; j++)
                            DTW[i][j] = 1000;
                    }
                    DTW[0][0] = 0;

                    for (int i = 1; i < 501; i++)
                    {
                        for (int j = 1; j < 501; j++)
                        {
                            float cost = fabs(temp->freq1[i - 1] - temp->freq2[j - 1]);
                            // printf("cost: %f",cost);
                            DTW[i][j] = cost + fmin(DTW[i][j - 1], fmin(DTW[i - 1][j], DTW[i - 1][j - 1]));
                            // printf(" DTW[%d][%d]: %f\n",i,j,DTW[i][j]);
                        }
                    }

                    printf("DTWdistance: %f\n", DTW[500][500]);
                    if (DTW[500][500] >= 35)
                    {
                        free(temp->freq2);
                        temp->result = 0;
                    }
                    else
                    {
                        long double *f = temp->freq1;
                        temp->freq1 = temp->freq2;
                        free(f);
                        temp->result = 1;
                    }
                    free(DTW);
                    temp->count_track1 = 1;
                }
                break;
            }
            previous = temp;
            temp = temp->next;
        }
        if (temp == NULL)
        {
            DTWfreqlist *bring = (DTWfreqlist *)malloc(sizeof(DTWfreqlist));
            bring->count_track1 = 0;
            bring->count_track2 = 0;
            bring->flag = 0;
            bring->idcode = to_intconvertor(df->idcode);
            bring->next = NULL;
            bring->result=1;
            previous->next = bring;
            return 1;
        }
        return temp->result;
    }
}

void DTWvolDistance(struct data_frame *df)
{
    if (headvol == NULL)
    {
        headvol = (DTWvollist *)malloc(sizeof(DTWvollist));
        headvol->count_track1 = 0;
        headvol->count_track2 = 0;
        headvol->flag = 0;
        headvol->idcode = to_intconvertor(df->idcode);
        headvol->next = NULL;
        headvol->result=1;
        return 1;
    }
    else
    {
        DTWvollist *temp = headvol;
        DTWvollist *previous;
        while (temp != NULL)
        {
            if (to_intconvertor(df->idcode) == temp->idcode)
            {
                float CURR_vol;
                if (df->dpmu[0]->fmt->phasor == '0')
                {
                    unsigned char* s1;
                    unsigned char* s2;
                    strncpy(s1,df->dpmu[0]->phasors[0],2);
                    strncpy(s2,df->dpmu[0]->phasors[0]+2,2);
                    long double v1 = to_intconvertor(s1);
                    long double v2 = to_intconvertor(s2);
                    CURR_vol = sqrt((v1*v1)+(v2*v2));
                }
                else
                {
                    unsigned char* s1;
                    unsigned char* s2;
                    strncpy(s1,df->dpmu[0]->phasors[0],4);
                    strncpy(s2,df->dpmu[0]->phasors[0]+2,4);
                    long double v1 = decode_ieee_single(s1);
                    long double v2 =decode_ieee_single(s2);
                    CURR_vol = sqrt((v1*v1)+(v2*v2));
                }

                // printf("count1: %d\ncount2: %d\n",count_track1,count_track2);
                // printf("curr_vol: %f\n",CURR_vol);
                if (temp->count_track1 == 0)
                {
                    if (tmp->flag == 0)
                    {
                        temp->flag = 1;
                        temp->vol1 = (long double *)malloc(500 * sizeof(long double));
                    }
                    temp->vol1[temp->count_track2] = CURR_vol;
                    temp->count_track2++;
                    if (temp->count_track2 == 500)
                    {
                        temp->count_track2 = 0;
                        temp->flag = 0;
                        temp->count_track1++;
                    }
                }
                else if (temp->count_track1 == 1)
                {
                    if (temp->flag == 0)
                    {
                        temp->flag = 1;
                        temp->vol2 = (long double *)malloc(500 * sizeof(long double));
                    }
                    temp->vol2[temp->count_track2] = CURR_vol;
                    temp->count_track2++;
                    if (temp->count_track2 == 500)
                    {
                        temp->count_track2 = 0;
                        temp->flag = 0;
                        temp->count_track1++;
                    }
                }
                else
                {
                    float **DTW = (float **)malloc(501 * sizeof(float *));
                    for (int i = 0; i < 501; i++)
                        DTW[i] = (float *)malloc(501 * sizeof(float));

                    for (int i = 0; i < 501; i++)
                    {
                        for (int j = 0; j < 501; j++)
                            DTW[i][j] = 1000;
                    }
                    DTW[0][0] = 0;

                    for (int i = 1; i < 501; i++)
                    {
                        for (int j = 1; j < 501; j++)
                        {
                            float cost = fabs(temp->vol1[i - 1] - temp->vol2[j - 1]);
                            // printf("cost: %f",cost);
                            DTW[i][j] = cost + fmin(DTW[i][j - 1], fmin(DTW[i - 1][j], DTW[i - 1][j - 1]));
                            // printf(" DTW[%d][%d]: %f\n",i,j,DTW[i][j]);
                        }
                    }

                    printf("DTWdistance: %f\n", DTW[500][500]);
                    if (DTW[500][500] >= 35)
                    {
                        free(temp->vol2);
                        temp->result = 0;
                    }
                    else
                    {
                        long double *f = temp->vol1;
                        temp->vol1 = temp->vol2;
                        free(f);
                        temp->result = 1;
                    }
                    free(DTW);
                    temp->count_track1 = 1;
                }
                break;
            }
            previous = temp;
            temp = temp->next;
        }
        if (temp == NULL)
        {
            DTWvollist *bring = (DTWvollist *)malloc(sizeof(DTWvollist));
            bring->count_track1 = 0;
            bring->count_track2 = 0;
            bring->flag = 0;
            bring->idcode = to_intconvertor(df->idcode);
            bring->next = NULL;
            bring->result=1;
            previous->next = bring;
            return 1;
        }
        return temp->result;
    }
}