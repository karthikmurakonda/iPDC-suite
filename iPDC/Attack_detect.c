/* pavan changes */
#include<time.h>
#include<stdio.h>
#include<stdlib.h>
#include "parser.h"

void attack_detect(struct data_frame *df,time_t* START,float* COUNT,float* SUM_OF_FREQUENCY)
{
    // printf("freq: %d\n",to_intconvertor(&(df->dpmu[0]->fmt->freq)));
    // printf("analog: %d\n",to_intconvertor(&(df->dpmu[0]->fmt->analog)));
    // printf("phasor %d\n",to_intconvertor(&(df->dpmu[0]->fmt->phasor)));
    // printf("polar: %d\n",to_intconvertor(&(df->dpmu[0]->fmt->polar)));

	*COUNT = *COUNT+1;
    float CURR_FREQ=to_intconvertor(df->dpmu[0]->freq);
	*SUM_OF_FREQUENCY+=CURR_FREQ;
	float FREQ_AVG=*SUM_OF_FREQUENCY/(*COUNT*1.0f);
	float DETECT_PERCENT=(abs(FREQ_AVG-CURR_FREQ)/(FREQ_AVG*1.0f))*100;

    /* detecting based on thershold */
    float THRESHOLD=70;
	if(DETECT_PERCENT>THRESHOLD)
    {
        printf("\033[0;31m");
        printf("ATTACK DETECTED!");
        printf("\033[0m");
        printf(" Detect_percent: %f\n",DETECT_PERCENT);
    }
	else
    {
        printf("\033[0;32m");
        printf("NO PROBLEM :)\n");
        printf("\033[0m"); 
    }
		
	/* calcculating time */
	if(*COUNT==1)
    {
        time(START);
        printf("entered\n");
    }
	time_t END;
	time(&END);
    double time_used = difftime(END,*START);
	printf("time used %lf\n",time_used);
	
    /* resetting after i minute */
    if(time_used > 60)
    {
        time(START);
        *SUM_OF_FREQUENCY=CURR_FREQ;
        *COUNT=1;
    }
}

/* pavan changes */