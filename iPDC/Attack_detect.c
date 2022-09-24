#include "Attack_detect.h"
#include "parser.h"

/* pavan changes */
void attack_detect(struct data_frame *df,clock_t START)
{
    float CURR_FREQ=to_intconvertor(df->dpmu[0]->freq);
	SUM_OF_FREQUENCY+=CURR_FREQ;
	COUNT=0;
	float FREQ_AVG=SUM_OF_FREQUENCY/(COUNT*1.0f);
	float DETECT_PERCENT=(abs(FREQ_AVG-CURR_FREQ)/(FREQ_AVG*1.0f))*100;
    double END = clock();

	printf("Detect_percent: %f\n",DETECT_PERCENT);

	if(DETECT_PERCENT>THRESHOLD)
		printf("ATTACK DETECTED\n");
	else
		printf("NO PROBLEM\n");

    cpu_time_used = ((double) (END - START)) / CLOCKS_PER_SEC;

    if(cpu_time_used > 10)
    {
        START = 0;
        SUM_OF_FREQUENCY=CURR_FREQ;
        COUNT=1;
    }
}
/* pavan changes */