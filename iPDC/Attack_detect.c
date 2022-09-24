/* pavan changes */
#include<time.h>
#include<stdio.h>
#include<stdlib.h>
#include "parser.h"

void attack_detect(struct data_frame *df,time_t* START,float* COUNT,float* SUM_OF_FREQUENCY)
{
	*COUNT = *COUNT+1;
    printf("count: %f\n",*COUNT);
    float CURR_FREQ=to_intconvertor(df->dpmu[0]->freq);
	*SUM_OF_FREQUENCY+=CURR_FREQ;
	float FREQ_AVG=*SUM_OF_FREQUENCY/(*COUNT*1.0f);
	float DETECT_PERCENT=(abs(FREQ_AVG-CURR_FREQ)/(FREQ_AVG*1.0f))*100;

	printf("Detect_percent: %f\n",DETECT_PERCENT);
	printf("frequency average: %f\n",FREQ_AVG);

    float THRESHOLD=25;
	if(DETECT_PERCENT>THRESHOLD)
		printf("ATTACK DETECTED\n");
	else
		printf("NO PROBLEM\n");
	
	if(*COUNT==1)
    {
        time(START);
        printf("entered\n");
    }
		

	time_t END;
	time(&END);
	
    double time_used = difftime(END,*START);
	printf("time used %lf\n",time_used);
	

    if(time_used > 60)
    {
        time(START);
        *SUM_OF_FREQUENCY=CURR_FREQ;
        *COUNT=1;
    }
}

/* pavan changes */