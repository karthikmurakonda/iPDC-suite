/* pavan changes */
#include <time.h>
#include <gtk/gtk.h>
/* variables declared of attack_detect function */
float  SUM_OF_FREQUENCY=0;
float COUNT=0;
time_t START;

/* function declared  */
gboolean attack_detect(struct data_frame *df,time_t* START,float* COUNT,float* SUM_OF_FREQUENCY);
/* pavan changes */