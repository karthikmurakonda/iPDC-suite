/*Pavan Changes*/

#include "parser.h"
#include <stdlib.h>
#include <gtk/gtk.h>

gboolean kmeans(struct data_frame *df,unsigned long long int *count_A,unsigned long long int *count_B,
unsigned long long int *count_C,long double *A,long double *B,long double *C)
{
    float CURR_FREQ = to_intconvertor(df->dpmu[0]->freq);
    long double diff_A = fabs(*A-CURR_FREQ);
    long double diff_B = fabs(*B-CURR_FREQ);
    long double diff_C = fabs(*C-CURR_FREQ);
    printf("A: %Lf, B: %Lf, C: %Lf\n",*A,*B,*C);
    if(diff_A <= diff_B && diff_A <= diff_C){
        *A = ((*count_A*(*A))+CURR_FREQ)/(++*count_A);
        return TRUE;
    }
    else if(diff_B < diff_C){
        *B = ((*count_B*(*B))+CURR_FREQ)/(++*count_B);
        return FALSE;
    }
    else{
        *C = ((*count_C*(*C))+CURR_FREQ)/(++*count_C);
        return FALSE;
    }
}

/*Pavan Changes*/