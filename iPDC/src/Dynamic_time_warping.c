#include "parser.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <gtk/gtk.h>
#include <stdlib.h>

struct DTWfreqlist
{
    int idcode;
    long double *freq1;
    long double *freq2;
    int count_track1;
    int count_track2;
    int flag;
    int result;
    struct DTWfreqlist *next;
};

struct DTWvollist
{
    int idcode;
    long double *vol1;
    long double *vol2;
    int count_track1;
    int count_track2;
    int flag;
    int result;
    struct DTWvollist *next;
};

struct DTWfreqlist *dtwhead = NULL;
struct DTWvollist *dtwheadvol = NULL;

int DTWfreqDistance(struct data_frame *df)
{
    if (dtwhead == NULL)
    {
        dtwhead = (struct DTWfreqlist *)malloc(sizeof(struct DTWfreqlist));
        dtwhead->count_track1 = 0;
        dtwhead->count_track2 = 0;
        dtwhead->flag = 0;
        dtwhead->idcode = to_intconvertor(df->idcode);
        dtwhead->next = NULL;
        dtwhead->result = 1;
        return 1;
    }
    else
    {
        struct DTWfreqlist *temp = dtwhead;
        struct DTWfreqlist *previous;
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
                    if (temp->flag == 0)
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
                return temp->result;
                break;
            }
            previous = temp;
            temp = temp->next;
        }
        if (temp == NULL)
        {
            struct DTWfreqlist *bring = (struct DTWfreqlist *)malloc(sizeof(struct DTWfreqlist));
            bring->count_track1 = 0;
            bring->count_track2 = 0;
            bring->flag = 0;
            bring->idcode = to_intconvertor(df->idcode);
            bring->next = NULL;
            bring->result = 1;
            previous->next = bring;
            return 1;
        }
    }
}

int DTWvolDistance(struct data_frame *df)
{
    if (dtwheadvol == NULL)
    {
        dtwheadvol = (struct DTWvollist *)malloc(sizeof(struct DTWvollist));
        dtwheadvol->count_track1 = 0;
        dtwheadvol->count_track2 = 0;
        dtwheadvol->flag = 0;
        dtwheadvol->idcode = to_intconvertor(df->idcode);
        dtwheadvol->next = NULL;
        dtwheadvol->result = 1;
        return 1;
    }
    else
    {
        struct DTWvollist *temp = dtwheadvol;
        struct DTWvollist *previous;
        while (temp != NULL)
        {
            if (to_intconvertor(df->idcode) == temp->idcode)
            {
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

                // printf("count1: %d\ncount2: %d\n",count_track1,count_track2);
                // printf("curr_vol: %f\n",CURR_vol);
                if (temp->count_track1 == 0)
                {
                    if (temp->flag == 0)
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
                return temp->result;
                break;
            }
            previous = temp;
            temp = temp->next;
        }
        if (temp == NULL)
        {
            struct DTWvollist *bring = (struct DTWvollist *)malloc(sizeof(struct DTWvollist));
            bring->count_track1 = 0;
            bring->count_track2 = 0;
            bring->flag = 0;
            bring->idcode = to_intconvertor(df->idcode);
            bring->next = NULL;
            bring->result = 1;
            previous->next = bring;
            return 1;
        }
    }
}

int DTWfreqvolDistance(struct data_frame *df)
{
    return DTWfreqDistance(df) && DTWfreqvolDistance(df);
}