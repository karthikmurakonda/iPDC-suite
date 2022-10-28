/* pavan changes */
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <math.h>
#include "parser.h"

long double AVERAGE_OF_FREQUENCY = 50;
unsigned long long int COUNT = 500;

gboolean attack_detect(struct data_frame *df)
{
    float CURR_FREQ = 50 + to_intconvertor(df->dpmu[0]->freq) * 1e-3;
    printf("Current freq: %f\n", CURR_FREQ);
    float DETECT_PERCENT = (fabs(AVERAGE_OF_FREQUENCY - CURR_FREQ) / (AVERAGE_OF_FREQUENCY * 1.0f)) * 100;

    /* detecting based on thershold frequency can vary only around 0.2 hz*/
    if (DETECT_PERCENT > 0.9)
    {
        printf("\033[0;31m");
        printf("ATTACK DETECTED!");
        printf("\033[0m");
        return FALSE;
    }
    else
    {
        printf("\033[0;32m");
        printf("NO PROBLEM :)\n");
        printf("\033[0m");

        /* updating mean of frequency */
        AVERAGE_OF_FREQUENCY = ((AVERAGE_OF_FREQUENCY * COUNT) + CURR_FREQ) / ++COUNT;
        printf("avg freq: %Lf\n", AVERAGE_OF_FREQUENCY);
        return TRUE;
    }
}

/* pavan changes */