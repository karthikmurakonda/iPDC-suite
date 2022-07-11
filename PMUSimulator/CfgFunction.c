/* ----------------------------------------------------------------------------- 
 * CfgFunction.c
 * 
 * PMU Simulator - Phasor Measurement Unit Simulator
 *
 * Copyright (C) 2011-2012 Nitesh Pandit
 * Copyright (C) 2011-2012 Kedar V. Khandeparkar
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * Authors: 
 *		Nitesh Pandit <panditnitesh@gmail.com>
 *		Kedar V. Khandeparkar <kedar.khandeparkar@gmail.com>			
 *
 * ----------------------------------------------------------------------------- */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <gtk/gtk.h>
#include "CfgFunction.h"
#include "function.h"
#include "PmuGui.h"
#include "ShearedMemoryStructure.h"

#define max_data_rate 200
#define MAX_STRING_SIZE 5000


/*----------------------------------------------------------------------------------------*/
/*                            Functions in CfgFunction.c        			               */
/*----------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/*                                                                    */
/*   1. void header_frm_gen(int len);                                 */
/*   2. void reconfig_cfg_CC();                                       */
/*   3. int  create_cfg();                                            */
/*   4. void show_pmu_details (GtkWidget *widget, gpointer udata);    */
/*                                                                    */
/*--------------------------------------------------------------------*/

/* ------------------------------------------------------------------ */
/*                            global variables                        */
/* ------------------------------------------------------------------ */

double t1;
int frmt, fdf, af, pf, pn, phnmr, annmr, dgnmr, num_pmu = 1; 
int cfg2_frm_size, data_frm_size = 18, data_rate, cfgcnt = 0;  		
int i, j, n, pmu_id, indx = 0, global_data_frm_size=0, hdrFrmSize=0;
long int soc, fracsec = 0;
long int phunit = 915527, anunit = 1, digunit = 0;

const long int TB = 16777215;  // roundf(powf(2,24) - 1);

char temp_5[16];
unsigned char temp[2], temp_1[4], temp_6[16];
unsigned char cfg2_frm[MAX_STRING_SIZE];
unsigned char header[MAX_STRING_SIZE];

struct timeval tim;
uint16_t chk;


/* ------------------------------------------------------------------------*/
/* FUNCTION  header_frm_gen(int len)                                       */
/* It generates the Header Frame from user given information about PMU and	*/
/* store in a PMU Setup File.                                              */
/* ------------------------------------------------------------------------*/

void header_frm_gen(int len)
{
	/* local variables */
	int i, indx = 0;
	FILE *fp1;

	memset(header, '\0', sizeof(header));

	/* Insert the fields in Header Frame: sync word */
	header[indx++] = 0xAA; 
	header[indx++] = 0x001; 

	/* Insert frame size in Header Frame */
	i2c(0, temp);
	B_copy(header, temp, indx, 2);
	indx = indx + 2;

	/* Insert user given PMU ID in Header Frame  */
	i2c(pmu_id, temp);
	B_copy(header, temp, indx, 2);
	indx = indx + 2;

	/* Insert current SOC value in Header Frame */				
	//time_t tm = time(NULL);
	gettimeofday(&tim, NULL);
	soc = (long) tim.tv_sec;
	li2c(soc, temp_1);
	B_copy(header, temp_1, indx, 4);
	indx = indx + 4;

	/* Insert the time quality fields & fraction of second value in Header Frame */
	//gettimeofday(&tim, NULL);
	//t1=tim.tv_sec+(tim.tv_usec/1000000.0);
	//fracsec = (t1-soc)*TB;
    fracsec = (long) (tim.tv_usec*TB/1e6);
	li2c(fracsec, temp_1);
	B_copy(header, temp_1, indx, 4);
	indx = indx + 4;

	/* Insert the PMU Information given by user in Header Frame */					
	for (i = 0; i < len; i++)
	{
		header[indx+i] = hdr_frame[i];
	}
	indx = indx + i;

	/* Calculation & insert the checksum VALUE of Header frame (up to now) */
	chk = compute_CRC(header,indx);
	header[indx++] = (chk >> 8) & ~(~0<<8);  	/* CHKSUM high byte */
	header[indx++] = (chk ) & ~(~0<<8);     	/* CHKSUM low byte */

	/* Insert frame size in Header Frame */
	header[2] = indx>>8;;
	header[3] = indx;

	/* Open the PMU Setup File */
	fp1 = fopen(pmuFilePath,"ab");

	if (fp1 != NULL)		
	{
		hdrFrmSize = indx;

		char buff[15];
		sprintf(buff, "%d", indx);

		/* Write the size of Header Frame & then Header Frame into file */
		fputs("HdrFrame 1 ", fp1);
		fputs(buff, fp1);

		fwrite(header, sizeof(unsigned char), indx, fp1);
		fputc('\n', fp1);

		fclose(fp1);

	     printf("\n-> Size of Header frame is %d Bytes.", indx);
	     validation_result (" Headre Frame successfully generated.");
	}
}; /* end of header_frm_gen() */   


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  reconfig_cfg_CC()                                                  */
/* This function allows user to easy modify the existing Configuration Frame.	*/
/* In terms of modification user can add new phasor channels or remove.		*/
/* It also provide the option for change Data Rate. Finally replace the new     */
/* Configuration frame with old CFG in PMU Setup File and gives signal to PMU   */
/* serverchange to change the appropriate bit of STAT Word while sending        */
/* Data Frames to connected PDC.                                                */
/* ----------------------------------------------------------------------------	*/

void reconfig_cfg_CC()
{
	/* local Variables*/
	unsigned char stn[16];
	int old_data_rate, indx=2; 			
	int j, global_frmt, tmp_k;        
	FILE *fp1;

	/* Get the Station name from CFG FRM */
	memset(stn, '\0', 16);
	strncpy((char *)stn, cfg_info->cfg_STNname, 16);

	/* Get the FORMAT word from CFG FRM */
	global_frmt = frmt;

	/* Ask user's choice what type of modification he wants? */
	if(new_cfg_info->add_remove_choice == 1)	/* Add more Phasor/Analog channels in existing CFG as per user's wish */
	{
		phnmr = cfg_info->cfg_phnmr_val + new_cfg_info->new_cfg_phnmr_val;
		annmr = cfg_info->cfg_annmr_val + new_cfg_info->new_cfg_annmr_val;
	}
	else if(new_cfg_info->add_remove_choice == 2)	/* Remove Phasor/Analog channels from existing CFG as per user's wish */
	{
		phnmr = cfg_info->cfg_phnmr_val - new_cfg_info->new_cfg_phnmr_val;
		annmr = cfg_info->cfg_annmr_val - new_cfg_info->new_cfg_annmr_val;
	}

	/* Calculate the size of new CFG Frame */
	cfg2_frm_size = 0;
	cfg2_frm_size = 54 + (16*phnmr) + (16*annmr) + (256*dgnmr) + (4*phnmr) + (4*annmr) + (4*dgnmr);

	/* Calculate the corresponding data frame size */
	global_data_frm_size = data_frm_size;
	data_frm_size = 0;
	data_frm_size = 18;

	/* Calculate 4/8 bytes for each PHNMR */
	if (cfg_info->cfg_pf == 0) 
	{
		data_frm_size = data_frm_size + (4*phnmr);
	}
	else 
	{
		data_frm_size = data_frm_size + (8*phnmr);
	}

	/* Calculate 2/4 bytes for each ANNMR */
	if (cfg_info->cfg_af == 0) 
	{
		data_frm_size = data_frm_size + (2*annmr);
	}
	else 
	{
		data_frm_size = data_frm_size + (4*annmr);
	}

	/* Calculate 2/4 bytes for both (FREQ + DFREQ) */
	if (cfg_info->cfg_fdf == 0) 
	{				
		data_frm_size = data_frm_size + 4;
	}
	else 
	{
		data_frm_size = data_frm_size + 8;
	}

	/* Calculate 2 bytes for each DGNMR */
	data_frm_size = data_frm_size + (2*dgnmr);
	printf("\nCFG = %d, Data Frame = %d\n", cfg2_frm_size, data_frm_size);

	/* Insert the fields in new CFG Frame: sync word */
	memset(cfg2_frm,'\0',sizeof(cfg2_frm));
	indx = 0;
	cfg2_frm[indx++] = 0xAA; 
	cfg2_frm[indx++] = 0x31; 

	/* Insert frame size in new CFG Frame */
	i2c(cfg2_frm_size, temp);
	B_copy(cfg2_frm, temp, indx, 2);
	indx = indx + 2;

	/* Insert user given PMU ID in new CFG Frame  */
	i2c(pmu_id, temp);
	B_copy(cfg2_frm, temp, indx, 2);
	indx = indx + 2;

	/* Insert current SOC value in new CFG Frame */				
	soc = 0;
	li2c(soc, temp_1);
	B_copy(cfg2_frm, temp_1, indx, 4);
	indx = indx + 4;

	/* Insert the time quality fields & fraction of second value in new CFG Frame, here it is static "00000000" */
	li2c(fracsec, temp_1);
	B_copy(cfg2_frm, temp_1, indx, 4);
	indx = indx + 4;

	/* Insert Time Base for calculating fraction of second in new CFG Frame, here it is static 1000000μs */
	li2c(TB, temp_1);
	B_copy(cfg2_frm, temp_1, indx, 4);
	indx = indx + 4;

	/* Insert number of PMU in new CFG Frame: static "0001" */
	i2c(num_pmu, temp);
	B_copy(cfg2_frm, temp, indx, 2);
	indx = indx + 2;

	/* Insert the station name or PMU name in new CFG Frame */
	B_copy(cfg2_frm, stn, indx, 16);
	indx = indx + 16;

	/* Insert again pmu id in new CFG Frame */					
	i2c(pmu_id, temp);
	B_copy(cfg2_frm, temp, indx, 2);
	indx = indx + 2;

	/* Insert format word in new CFG Frame */
	i2c(global_frmt, temp);
	B_copy(cfg2_frm, temp, indx, 2);
	indx = indx + 2;

	/* Insert number of phasor or PHNMR in new CFG Frame */
	i2c(phnmr, temp);
	B_copy(cfg2_frm, temp, indx, 2);
	indx = indx + 2;

	/* Insert number of analog or ANNMR in new CFG Frame */
	i2c(annmr, temp);
	B_copy(cfg2_frm, temp, indx, 2);
	indx = indx + 2;

	/* Insert number of Digital status word or DGNMR in new CFG Frame */
	i2c(dgnmr, temp);
	B_copy(cfg2_frm, temp, indx, 2);
	indx = indx + 2;

	/* Insert NEW Phasor channel Names (if any) in new CFG frame */ 
	if (new_cfg_info->add_remove_choice == 1)
	{
		/* First Insert old Phasor channels in new CFG frame */
		B_copy(cfg2_frm, (unsigned char *)cfg_info->cfg_phasor_channels, indx, cfg_info->cfg_phnmr_val*16);
		indx = indx + (cfg_info->cfg_phnmr_val*16);

		if (new_cfg_info->new_cfg_phnmr_val > 0)
		{
			/* Insert New Phasor channels (if any) in new CFG frame */
			B_copy(cfg2_frm, (unsigned char *)new_cfg_info->new_cfg_phasor_channels, indx, (new_cfg_info->new_cfg_phnmr_val*16));
			indx = indx + (new_cfg_info->new_cfg_phnmr_val*16);
			free(new_cfg_info->new_cfg_phasor_channels);
		}
		cfg_info->cfg_phnmr_val = cfg_info->cfg_phnmr_val + new_cfg_info->new_cfg_phnmr_val;

		/* First Insert old Analog channels in new CFG frame */
		B_copy(cfg2_frm, (unsigned char *)cfg_info->cfg_analog_channels, indx, cfg_info->cfg_annmr_val*16);
		indx = indx + (cfg_info->cfg_annmr_val*16);
		if (new_cfg_info->new_cfg_annmr_val > 0)
		{
			/* Insert New Analog channels (if any) in new CFG frame */
			B_copy(cfg2_frm, (unsigned char *)new_cfg_info->new_cfg_analog_channels, indx, (new_cfg_info->new_cfg_annmr_val*16));
			indx = indx + (new_cfg_info->new_cfg_annmr_val*16);
			free(new_cfg_info->new_cfg_analog_channels);
		}
		cfg_info->cfg_annmr_val = cfg_info->cfg_annmr_val + new_cfg_info->new_cfg_annmr_val;
	}

	/* Remove channel Names (from availables) from new CFG frame */ 
	else if (new_cfg_info->add_remove_choice == 2)
	{
		/* Remove Phasor channels (if any) in new CFG frame */ 
		cfg_info->cfg_phnmr_val = cfg_info->cfg_phnmr_val - new_cfg_info->new_cfg_phnmr_val;
		B_copy(cfg2_frm, (unsigned char *)cfg_info->cfg_phasor_channels, indx, cfg_info->cfg_phnmr_val*16);
		indx = indx + (cfg_info->cfg_phnmr_val*16);      		

		/* Remove Analog channels (if any) in new CFG frame */ 
		cfg_info->cfg_annmr_val = cfg_info->cfg_annmr_val - new_cfg_info->new_cfg_annmr_val;
		B_copy(cfg2_frm, (unsigned char *)cfg_info->cfg_analog_channels, indx, cfg_info->cfg_annmr_val*16);
		indx = indx + (cfg_info->cfg_annmr_val*16);
	}

	/* Insert Digital channel Names in new CFG frame */
	B_copy(cfg2_frm, (unsigned char *)cfg_info->cfg_digital_channels, indx, (cfg_info->cfg_dgnmr_val*256));
	indx = indx + (cfg_info->cfg_dgnmr_val*256);

	/* Insert FACTOR VALUES for Phasor, Analog and Digital in new CFG frame */
	for(j=0, tmp_k=0; j<cfg_info->cfg_phnmr_val; j++)			/* FACTOR VALUES for Phasor */
	{
		cfg2_frm[indx++] = phunit_final_val[tmp_k++];
		cfg2_frm[indx++] = phunit_final_val[tmp_k++];
		cfg2_frm[indx++] = phunit_final_val[tmp_k++];
		cfg2_frm[indx++] = phunit_final_val[tmp_k++];
	}
	for(j=0, tmp_k=0; j<cfg_info->cfg_annmr_val; j++)			/* FACTOR VALUES for Phasor */
	{
		cfg2_frm[indx++] = anunit_final_val[tmp_k++];
		cfg2_frm[indx++] = anunit_final_val[tmp_k++];
		cfg2_frm[indx++] = anunit_final_val[tmp_k++];
		cfg2_frm[indx++] = anunit_final_val[tmp_k++];
	}

	for(j=0; j<cfg_info->cfg_dgnmr_val; j++)		/* FACTOR VALUES for Digital */
	{
		li2c(digunit, temp_1);
		B_copy(cfg2_frm, temp_1, indx, 4);
		indx = indx + 4;
	}

	/* Insert normal frequency or fnom VALUE in new CFG frame */
	i2c(cfg_info->cfg_fnom, temp);
	B_copy(cfg2_frm, temp, indx, 2);
	indx = indx + 2;

	/* Data rate change? option for user */
	old_data_rate = data_rate;
	if (new_cfg_info->data_rate_choice == 1)
	{
		data_rate = new_cfg_info->new_data_rate;
		cfg_info->cfg_dataRate = new_cfg_info->new_data_rate;			
		printf("\n-> New Data Rate = %d\n", data_rate);
	}

	/* Insert configuration count or cfgcnt VALUE in new CFG frame is any of addition or removal of channel happened??? */
	if ((global_data_frm_size != data_frm_size) || (old_data_rate != data_rate)) 
	{
		cfgcnt = cfgcnt + 1;
		printf("\n-> CFG COUNT incremented = %d\n", cfgcnt);
		i2c(cfgcnt, temp);
		B_copy(cfg2_frm, temp, indx, 2);
		indx = indx + 2;
	}
	else
	{ 
		i2c(cfgcnt, temp);
		B_copy(cfg2_frm, temp, indx, 2);
		indx = indx + 2;
	}

	/* Insert Data Rate VALUE in new CFG frame */
	i2c(data_rate, temp);
	B_copy(cfg2_frm, temp, indx, 2);
	indx = indx + 2;

	/* Insert current SOC value in new CFG Frame */				
    gettimeofday(&tim, NULL);
    soc = (long) tim.tv_sec;
    li2c(soc, temp_1);
    B_copy(cfg2_frm, temp_1, 6, 4);

    /* Insert the time quality fields & fraction of second value in new CFG Frame */
    //gettimeofday(&tim, NULL);
    //t1=tim.tv_sec+(tim.tv_usec/1000000.0);
    //fracsec = (t1-soc)*TB;
    fracsec = (long) (tim.tv_usec*TB/1e6);
    li2c(fracsec, temp_1);
    B_copy(cfg2_frm, temp_1, 10, 4);

	/* Calculation & insert the checksum VALUE of new CFG frame (up to now) */
	chk = compute_CRC(cfg2_frm,indx);
	cfg2_frm[indx++] = (chk >> 8) & ~(~0<<8);  	/* CHKSUM high byte */
	cfg2_frm[indx++] = (chk ) & ~(~0<<8);     	/* CHKSUM low byte */

	/* Open the PMU Setup File and replace new CFG frame with old CFG if and only if 
        when last two calculated data frame are not equal or data rate changed */ 
	if ((global_data_frm_size != data_frm_size) || (old_data_rate != data_rate)) 
	{
          /* Delete the old PMU Setup File and create new PMU Setup File with new CFG frame */
		if (remove(pmuFilePath) == -1)
			perror("Error in deleting a file");

		fp1 = fopen (pmuFilePath,"wb");

		char buff[50];

		sprintf(buff, "%d", PMU_uport);
		fputs(buff, fp1);
		fputc('\n', fp1);

		sprintf(buff, "%d", PMU_tport);
		fputs(buff, fp1);
		fputc('\n', fp1);

		fputs("CFG 1 ", fp1);
		sprintf(buff, "%d", indx);
		fputs(buff, fp1);
		fputc('\n', fp1);

		fwrite(cfg2_frm, sizeof(unsigned char),indx,fp1);
		fputc('\n', fp1);

		if(hdrFrmSize > 0)
		{
			fputs("HDR 1 ", fp1);
			sprintf(buff, "%d", hdrFrmSize);
			fputs(buff, fp1);
			fputc('\n', fp1);

			fwrite(header, sizeof(unsigned char),hdrFrmSize,fp1);
			fputc('\n', fp1);
		}
		else
		{
			fputs("HDR 0 0", fp1);
			fputc('\n', fp1);
		}

		fclose (fp1);
		free(new_cfg_info);
		validation_result ("Modification in Configuration frame has been successfully updated.");

          	/* Give signal to PMU Server for change in configuration frame */
		ShmPTR->cfg_bit_change_info = 1;
		p1.pid = ShmPTR->pid;

		kill(p1.pid, SIGUSR2);
		printf("Configuration frame modification signal sent to PMU Server.\n");

		/* Set raw data selection as configuration has been changed */
		strcpy(ShmPTR->filePath, "nil");
		ShmPTR->dataFileVar = 0;
		p1.pid = ShmPTR->pid;

		kill(p1.pid, SIGUSR1);
		printf("Set Raw Measurements in Data Frames because configuration of PMU has been changed.\n");

	}
	else		/* If CFG is not modified */
	{
		validation_result ("No modification has been done in Configuration frame.");
	}

	printf("\n-> New configuration frame has been generated of size %d Bytes.\nAnd the new data frame size would be %d Bytes.\n\n[", cfg2_frm_size, data_frm_size);

} /* end of reconfig_cfg_CC() */


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  create_cfg()                                                       */
/* It generate the Configuration Frame as per user entered information.		*/
/* It write in the file PMU Setup File.                                         */
/* It will also reload the saved PMU Setup File give by user.                   */
/* ----------------------------------------------------------------------------	*/

int create_cfg()
{
	/* local Variables*/
	int  j, a, x1, tmp_k;
	int tempi, indx = 0;
	char stn[17],*rline = NULL, *d1;
	unsigned char cline[MAX_STRING_SIZE];

	size_t len = 0;
	ssize_t read;
	FILE *fp1;

	/* Open the saved PMU Setup File and read the CFG frame if any? */
	fp1 = fopen (pmuFilePath,"rb");

	if (fp1 != NULL)
	{
		tempi = 1;
		while(tempi < 4)
		{
			read = getline(&rline, &len, fp1);

			if(read == 0)
				break;

			tempi++;
		}

		if(read > 0)
		{
			d1 = strtok (rline," ");
			d1 = strtok (NULL," ");
			tempi = atoi(d1);

			if (tempi == 1)
			{
				d1 = strtok (NULL,"\n");
				tempi = atoi(d1);

				/* Copy the full Configuration Frame into 'cline' array */
				memset(cline,'\0',sizeof(cline));
				fread(cline, sizeof(unsigned char), tempi, fp1);
				fclose(fp1);

				/* Allocate the memory for ConfigurationFrame object */
				cfg_info = malloc(sizeof(struct ConfigurationFrame));
				cfg_info->cfg_STNname = malloc(17);

				/* Get the CFG size & store globaly */
				temp[0] = cline[2];
				temp[1] = cline[3];
				cfg2_frm_size = c2i(temp);

				/* Get the PMU ID from CFG FRM & store globally */
				temp[0] = cline[4];
				temp[1] = cline[5];
				pmu_id = c2i(temp);
				cfg_info->cfg_pmuID = pmu_id;

				/* Get the PMU ID from CFG FRM & store globally */
				temp_1[0] = cline[14];
				temp_1[1] = cline[15];
				temp_1[2] = cline[16];
				temp_1[3] = cline[17];
				//TB = c2li(temp_1);

				/* Get the Station name from CFG FRM & store globally */
				for(a=0; a<16; a++)
				{
					stn[a] = cline[20+a];
				}
				stn[16] = '\0';
				strcpy(cfg_info->cfg_STNname, stn);

				/* Get the FORMAT word from CFG FRM */
				temp[0] = cline[38];
				temp[1] = cline[39];
				frmt = c2i(temp);

				/* Initialize the frmt bits as in his appropriate global variable */
				if(frmt == 15)
				{
					fdf=1, af=1, pf=1, pn=1;
				}
				else if(frmt == 14)
				{
					fdf=1, af=1, pf=1, pn=0;
				}
				else if(frmt == 13)
				{
					fdf=1, af=1, pf=0, pn=1;
				}
				else if(frmt == 12)
				{
					fdf=1, af=1, pf=0, pn=0;
				}
				else if(frmt == 11)
				{
					fdf=1, af=0, pf=1, pn=1;
				}
				else if(frmt == 10)
				{
					fdf=1, af=0, pf=1, pn=0;
				}
				else if(frmt == 9)
				{
					fdf=1, af=0, pf=0, pn=1;
				}
				else if(frmt == 8)
				{
					fdf=1, af=0, pf=0, pn=0;
				}
				else if(frmt == 7)
				{
					fdf=0, af=1, pf=1, pn=1;
				}
				else if(frmt == 6)
				{
					fdf=0, af=1, pf=1, pn=0;
				}
				else if(frmt == 5)
				{
					fdf=0, af=1, pf=0, pn=1;
				}
				else if(frmt == 4)
				{
					fdf=0, af=1, pf=0, pn=0;
				}
				else if(frmt == 3)
				{
					fdf=0, af=0, pf=1, pn=1;
				}
				else if(frmt == 2)
				{
					fdf=0, af=0, pf=1, pn=0;
				}
				else if(frmt == 1)
				{
					fdf=0, af=0, pf=0, pn=1;
				}
				else
				{
					fdf=0, af=0, pf=0, pn=0;
				}

				/* Store FORMAT in global data structure */
				cfg_info->cfg_fdf = fdf;
				cfg_info->cfg_af = af;
				cfg_info->cfg_pf = pf;
				cfg_info->cfg_pn = pn;

				/* Get the PHNMR from CFG FRM & store globally */
				temp[0] = cline[40];
				temp[1] = cline[41];
				phnmr = c2i(temp);
				cfg_info->cfg_phnmr_val = phnmr;

				/* Get the ANNMR from CFG FRM & store in globally */
				temp[0] = cline[42];
				temp[1] = cline[43];
				annmr = c2i(temp);
				cfg_info->cfg_annmr_val = annmr;

				/* Get the DGNMR from CFG FRM & store globally */
				temp[0] = cline[44];
				temp[1] = cline[45];
				dgnmr = c2i(temp);
				cfg_info->cfg_dgnmr_val = dgnmr;

				/* To escape the some of fields in cfg frame */
				indx = 46 + (16*phnmr) + (16*annmr) + (256*dgnmr) + (4*phnmr) + (4*annmr) + (4*dgnmr) + 2;
				temp[0] = cline[indx++];
				temp[1] = cline[indx++];
				cfgcnt = c2i(temp);

				/* Get the Data Rate from CFG FRM & store globally */
				temp[0] = cline[indx++];
				temp[1] = cline[indx++];
				data_rate = c2i(temp);
				cfg_info->cfg_dataRate = data_rate;

				/* Seprate the Phasor Channels from CFG FRM & store globally */
				cfg_info->cfg_phasor_channels = (char *)malloc((phnmr*16) * sizeof(char));

				for(x1=0, indx=46; x1< (phnmr*16); x1++,indx++)
				{
					cfg_info->cfg_phasor_channels[x1] = cline[indx];
				}

				/* Seprate the Analog Channels from CFG FRM & store globally */
				cfg_info->cfg_analog_channels = (char *)malloc((annmr*16) * sizeof(char));

				for(x1=0, indx; x1< (annmr*16); x1++,indx++)
				{
					cfg_info->cfg_analog_channels[x1] = cline[indx];
				}

				/* Seprate the Digital Channels from CFG FRM & store globally */
				cfg_info->cfg_digital_channels = (char *)malloc((dgnmr*16*16) * sizeof(char));

				for(x1=0, indx; x1< (dgnmr*16*16); x1++,indx++)
				{
					cfg_info->cfg_digital_channels[x1] = cline[indx];
				}

				/* Insert FACTOR VALUES for Phasor, Analog and Digital in new CFG frame */
				for(j=0, tmp_k=0; j<cfg_info->cfg_phnmr_val; j++)/* FACTOR VALUES for Phasor */
				{
					phunit_final_val[tmp_k++] = cline[indx++];
					phunit_final_val[tmp_k++] = cline[indx++];
					phunit_final_val[tmp_k++] = cline[indx++];
					phunit_final_val[tmp_k++] = cline[indx++];
				}
				for(j=0, tmp_k=0; j<cfg_info->cfg_annmr_val; j++)/* FACTOR VALUES for Analog */
				{
					anunit_final_val[tmp_k++] = cline[indx++];
					anunit_final_val[tmp_k++] = cline[indx++];
					anunit_final_val[tmp_k++] = cline[indx++];
					anunit_final_val[tmp_k++] = cline[indx++];
				}
			}
		}
	}
	else 	/* At the time of creating new PMU Setup File */
	{
		/* Store FORMAT bits from global data structure */
		fdf = cfg_info->cfg_fdf;
		af  = cfg_info->cfg_af;
		pf  = cfg_info->cfg_pf;
		pn  = cfg_info->cfg_pn;

		if(fdf==1 && af==1 && pf==1 && pn==1)
		{
			frmt = 15;
		}
		else if(fdf==1 && af==1 && pf==1 && pn==0)
		{
			frmt = 14;
		}
		else if(fdf==1 && af==1 && pf==0 && pn==1)
		{
			frmt = 13;
		}
		else if(fdf==1 && af==1 && pf==0 && pn==0)
		{
			frmt = 12;
		}
		else if(fdf==1 && af==0 && pf==1 && pn==1)
		{
			frmt = 11;
		}
		else if(fdf==1 && af==0 && pf==1 && pn==0)
		{
			frmt = 10;
		}
		else if(fdf==1 && af==0 && pf==0 && pn==1)
		{
			frmt = 9;
		}
		else if(fdf==1 && af==0 && pf==0 && pn==0)
		{
			frmt = 8;
		}
		else if(fdf==0 && af==1 && pf==1 && pn==1)
		{
			frmt = 7;
		}
		else if(fdf==0 && af==1 && pf==1 && pn==0)
		{
			frmt = 6;
		}
		else if(fdf==0 && af==1 && pf==0 && pn==1)
		{
			frmt = 5;
		}
		else if(fdf==0 && af==1 && pf==0 && pn==0)
		{
			frmt = 4;
		}
		else if(fdf==0 && af==0 && pf==1 && pn==1)
		{
			frmt = 3;
		}
		else if(fdf==0 && af==0 && pf==1 && pn==0)
		{
			frmt = 2;
		}
		else if(fdf==0 && af==0 && pf==0 && pn==1)
		{
			frmt = 1;
		}
		else
		{
			frmt = 0;
		}

		/* Accept Number of Phasor values in PHNMR */
		phnmr = cfg_info->cfg_phnmr_val;

		/* Accept Number of Phasor values in ANNMR */
		annmr = cfg_info->cfg_annmr_val;

		/* Accept Number of Phasor values in DGNMR */
		dgnmr = cfg_info->cfg_dgnmr_val;

		/* Calculate the total size of CFG Frame */
		cfg2_frm_size = 54 + (16*phnmr) + (16*annmr) + (4*phnmr) + (4*annmr) + (4*dgnmr) + (256*dgnmr);

		/* Calculate 4/8 bytes for each PHNMR & store globally */
		if (pf == 0)
		{
			data_frm_size = data_frm_size + (4*phnmr);
		}
		else
		{
			data_frm_size = data_frm_size + (8*phnmr);
		}

		/* Calculate 2/4 bytes for each ANNMR & store globally */
		if (af == 0)
		{
			data_frm_size = data_frm_size + (2*annmr);
		}
		else
		{
			data_frm_size = data_frm_size + (4*annmr);
		}

		/* Calculate 2/4 bytes for both (FREQ + DFREQ) & store globally */
		if (fdf == 0)
		{
			data_frm_size = data_frm_size + 4;
		}
		else
		{
			data_frm_size = data_frm_size + 8;
		}

		/* Calculate 2 bytes for each DGNMR & store globally */
		data_frm_size = data_frm_size + (2*dgnmr);

		/* Insert the fields in CFG Frame: sync word */
		memset(cfg2_frm,'\0',sizeof(cfg2_frm));
		cfg2_frm[indx++] = 0xAA;
		cfg2_frm[indx++] = 0x31;

		/* Insert frame size in CFG Frame */
		i2c(cfg2_frm_size, temp);
		B_copy(cfg2_frm, temp, indx, 2);
		indx = indx + 2;

		/* Insert user given PMU ID in CFG Frame  */
		pmu_id = cfg_info->cfg_pmuID;
		i2c(pmu_id, temp);
		B_copy(cfg2_frm, temp, indx, 2);
		indx = indx + 2;

		/* Insert SOC value in CFG Frame */
		soc = 0;
		li2c(soc, temp_1);
		B_copy(cfg2_frm, temp_1, indx, 4);
		indx = indx + 4;

		/* Insert the time quality fields & fraction of second value in CFG Frame, right now it is '00000000' */
		li2c(fracsec, temp_1);
		B_copy(cfg2_frm, temp_1, indx, 4);
		indx = indx + 4;

		/* Insert Time Base for calculating fraction of second in CFG Frame, here it is static 1000000μs */
		li2c(TB, temp_1);
		B_copy(cfg2_frm, temp_1, indx, 4);
		indx = indx + 4;

		/* Insert number of PMU in CFG Frame: Static "0001" */
		i2c(num_pmu, temp);
		B_copy(cfg2_frm, temp, indx, 2);
		indx = indx + 2;

		/* Insert the station name or PMU name in CFG Frame */
		strncpy(temp_5, (const char *)cfg_info->cfg_STNname, 16);
		B_copy(cfg2_frm, (unsigned char *)temp_5, indx, 16);
		indx = indx + 16;

		/* Again insert pmu id in CFG Frame */
		i2c(pmu_id, temp);
		B_copy(cfg2_frm, temp, indx, 2);
		indx = indx + 2;

		/* Insert format word in CFG Frame */
		i2c(frmt, temp);
		B_copy(cfg2_frm, temp, indx, 2);
		indx = indx + 2;

		/* Insert number of phasor or PHNMR in CFG Frame */
		i2c(phnmr, temp);
		B_copy(cfg2_frm, temp, indx, 2);
		indx = indx + 2;

		/* Insert number of analog or ANNMR in CFG Frame */
		i2c(annmr, temp);
		B_copy(cfg2_frm, temp, indx, 2);
		indx = indx + 2;

		/* Insert number of Digital status word or DGNMR in CFG Frame */
		i2c(dgnmr, temp);
		B_copy(cfg2_frm, temp, indx, 2);
		indx = indx + 2;

		/* All channel names or CHNAM fields in CFG frame. */
		/* Insert phasor channel names in CFG frame */
		B_copy(cfg2_frm, (unsigned char *)cfg_info->cfg_phasor_channels, indx, 16*phnmr);
		indx = indx + 16*phnmr;

		/* Insert Digital channel names in CFG frame */
		B_copy(cfg2_frm, (unsigned char *)cfg_info->cfg_analog_channels, indx, 16*annmr);
		indx = indx + 16*annmr;

		/* Insert Digital channel names in CFG frame */
		B_copy(cfg2_frm, (unsigned char *)cfg_info->cfg_digital_channels, indx, 16*16*dgnmr);
		indx = indx + 16*16*dgnmr;

		/* Insert FACTOR VALUES for Phasor, Analog and Digital in CFG frame */
		for(j=0, tmp_k=0; j<phnmr; j++)			/* FACTOR VALUES for Phasor */
		{
			cfg2_frm[indx++] = phunit_final_val[tmp_k++];
			cfg2_frm[indx++] = phunit_final_val[tmp_k++];
			cfg2_frm[indx++] = phunit_final_val[tmp_k++];
			cfg2_frm[indx++] = phunit_final_val[tmp_k++];
		}

		for(j=0, tmp_k=0; j<annmr; j++)			/* FACTOR VALUES for Analog */
		{
			cfg2_frm[indx++] = anunit_final_val[tmp_k++];
			cfg2_frm[indx++] = anunit_final_val[tmp_k++];
			cfg2_frm[indx++] = anunit_final_val[tmp_k++];
			cfg2_frm[indx++] = anunit_final_val[tmp_k++];
		}

		for(j=0; j< dgnmr; j++)			/* FACTOR VALUES for Digital */
		{
			li2c(digunit, temp_1);
			B_copy(cfg2_frm, temp_1, indx, 4);
			indx = indx + 4;
		}

		/* Insert normal frequency or fnom VALUE in CFG frame */
		i2c(cfg_info->cfg_fnom, temp);
		B_copy(cfg2_frm, temp, indx, 2);
		indx = indx + 2;

		/* Insert configuration count or cfgcnt VALUE in CFG frame */
		i2c(cfgcnt, temp);
		B_copy(cfg2_frm, temp, indx, 2);
		indx = indx + 2;

		/* Insert Data Rate in CFG frame */
		data_rate = cfg_info->cfg_dataRate;
		i2c(data_rate, temp);
		B_copy(cfg2_frm, temp, indx, 2);
		indx = indx + 2;

        /* Insert current SOC value in CFG Frame */
        gettimeofday(&tim, NULL);
        soc = (long) tim.tv_sec;
        li2c(soc, temp_1);
        B_copy(cfg2_frm, temp_1, 6, 4);

        /* Insert the time quality fields & fraction of second value in CFG Frame */
        //	gettimeofday(&tim, NULL);
        //	t1=tim.tv_sec+(tim.tv_usec/1000000.0);
        //	fracsec = (t1-soc)*TB;
        fracsec = (long) (tim.tv_usec*TB/1e6);
        li2c(fracsec, temp_1);
        B_copy(cfg2_frm, temp_1, 10, 4);

		/* Calculation & insert the checksum VALUE of new CFG frame (up to now) */
		chk = compute_CRC(cfg2_frm,indx);

		cfg2_frm[indx++] = (chk >> 8) & ~(~0<<8);  	/* CHKSUM high byte */
		cfg2_frm[indx++] = (chk ) & ~(~0<<8);     	/* CHKSUM low byte  */

		char buff[50];

          /* Create a new PMU Setup File and save the info with 'pmuPMU-ID.txt' name */
		memset(pmuFilePath, '\0', 200);
		strcpy(pmuFilePath, pmuFolderPath);
		strcat(pmuFilePath, "/");
		strcat(pmuFilePath, "pmu");
		sprintf(buff, "%d", pmu_id);
		strcat(pmuFilePath, buff);
		strcat(pmuFilePath, ".bin");
		pmuFilePath[strlen(pmuFilePath)] = '\0';

		fp1 = fopen (pmuFilePath,"wb");

		sprintf(buff, "%d", PMU_uport);
		fputs(buff, fp1);
		fputc('\n', fp1);

		sprintf(buff, "%d", PMU_tport);
		fputs(buff, fp1);
		fputc('\n', fp1);

		fputs("CFG 1 ", fp1);
		sprintf(buff, "%d", indx);
		fputs(buff, fp1);
		fputc('\n', fp1);

		fwrite(cfg2_frm, sizeof(unsigned char),indx,fp1);
		fputc('\n', fp1);

		fclose (fp1);

		printf("\nCFG Frame size = %d-Bytes, ", cfg2_frm_size);
		printf("& Data Frame size = %d-Bytes.\n", data_frm_size);
	}

     /* Now send a signal for CFG file path to PMU Server process*/
	strcpy(ShmPTR->cfgPath, pmuFilePath);
	ShmPTR->dataFileVar = 2;

	p1.pid = ShmPTR->pid;
	kill(p1.pid, SIGUSR1);
	printf("\nConfiguration file path has been sent to PMU Server through signal.\n");

	return 0;
} /* end of function() */


/* ----------------------------------------------------------------------------	*/
/* FUNCTION  show_pmu_details (GtkWidget *widget, gpointer udata)               */
/* It display the details of PMU Server and Configuartion details.              */
/* ----------------------------------------------------------------------------	*/

void show_pmu_details (GtkWidget *widget, gpointer udata)
{
	/* local variables */
	char tmpBuffer[30];
	GtkWidget *label, *table, *close_but;

	/* Create a new dialog window for the scrolled window to be packed into */  				
	new_window = gtk_dialog_new ();
	g_signal_connect (new_window, "destroy", G_CALLBACK (gtk_widget_destroy), new_window);
	gtk_window_set_title (GTK_WINDOW (new_window), "PMU Properties");
	gtk_container_set_border_width (GTK_CONTAINER (new_window), 10);

	/* Create a table of 14 by 2 squares. */
	table = gtk_table_new (16, 2, FALSE);

	/* Set the spacing to 35 on x and 25 on y */
	gtk_table_set_row_spacings (GTK_TABLE (table), 8);
	gtk_table_set_col_spacings (GTK_TABLE (table), 5);

	/* Pack the table into the scrolled window */
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG(new_window)->vbox), table, TRUE, TRUE, 0);
	gtk_widget_show (table);

	/* Add a "OK" button to the bottom of the dialog */
	close_but = gtk_button_new_with_label ("OK");

	/* This simply creates a grid of toggle buttons on the table to demonstrate the scrolled window */
	label = gtk_label_new (" ");
	markup1 = g_markup_printf_escaped ("<span foreground=\"#0000FF\" font='10'><b>PMU Server Details</b></span>");
	gtk_label_set_markup (GTK_LABEL (label), markup1);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 2, 0, 1);
	gtk_widget_show (label);
	g_free (markup1);

	label = gtk_label_new ("UDP Port");
     	gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 1, 2);
	gtk_widget_show (label);

	label = gtk_label_new ("TCP Port");
     	gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 2, 3);
	gtk_widget_show (label);

	/* We have the global variables to hold the port & protocol values */
	memset(tmpBuffer,'\0',30);
	sprintf(tmpBuffer,"%d",PMU_uport);
	label = gtk_label_new (tmpBuffer);
     	gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 1, 2, 1, 2);
	gtk_widget_show (label);

	memset(tmpBuffer,'\0',30);
	sprintf(tmpBuffer,"%d",PMU_tport);
	label = gtk_label_new (tmpBuffer);
     	gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 1, 2, 2, 3);
	gtk_widget_show (label);

	/* This simply creates a grid of toggle buttons on the table to demonstrate the scrolled window */
	label = gtk_label_new (" ");
	markup1 = g_markup_printf_escaped ("<span foreground=\"#0000FF\" font='10'><b>PMU Configuration Details</b></span>");
	gtk_label_set_markup (GTK_LABEL (label), markup1);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 2, 4, 5);
	gtk_widget_show (label);
	g_free (markup1);

	label = gtk_label_new ("PMU ID");
     	gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 5, 6);
	gtk_widget_show (label);

	label = gtk_label_new ("Station Name");
     	gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 6, 7);
	gtk_widget_show (label);

	label = gtk_label_new ("Number of Phasors");
     	gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 7, 8);
	gtk_widget_show (label);

	label = gtk_label_new ("Number of Analog ");
     	gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 8, 9);
	gtk_widget_show (label);

	label = gtk_label_new ("Digital Status Word");
     	gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 9, 10);
	gtk_widget_show (label);

	label = gtk_label_new ("Data Rate");
     	gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 10, 11);
	gtk_widget_show (label);

	label = gtk_label_new ("Format Word");
     	gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 11, 12);
	gtk_widget_show (label);

	label = gtk_label_new ("Configuration Count ");
     	gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 12, 13);
	gtk_widget_show (label);

	label = gtk_label_new ("CFG Frame Size");
     	gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 13, 14);
	gtk_widget_show (label);

     /* Printing the appropriate values from CFG objects */
	memset(tmpBuffer,'\0',30);
	sprintf(tmpBuffer,"%d",cfg_info->cfg_pmuID);
	label = gtk_label_new (tmpBuffer);
     	gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 1, 2, 5, 6);
	gtk_widget_show (label);

	memset(tmpBuffer,'\0',30);
	label = gtk_label_new (cfg_info->cfg_STNname);
     	gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 1, 2, 6, 7);
	gtk_widget_show (label);

	memset(tmpBuffer,'\0',30);
	sprintf(tmpBuffer,"%d",cfg_info->cfg_phnmr_val);
	label = gtk_label_new (tmpBuffer);
     	gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 1, 2, 7, 8);
	gtk_widget_show (label);

	memset(tmpBuffer,'\0',30);
	sprintf(tmpBuffer,"%d",cfg_info->cfg_annmr_val);
	label = gtk_label_new (tmpBuffer);
     	gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 1, 2, 8, 9);
	gtk_widget_show (label);

	memset(tmpBuffer,'\0',30);
	sprintf(tmpBuffer,"%d",cfg_info->cfg_dgnmr_val);
	label = gtk_label_new (tmpBuffer);
     	gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 1, 2, 9, 10);
	gtk_widget_show (label);

	memset(tmpBuffer,'\0',30);
	sprintf(tmpBuffer,"%d",cfg_info->cfg_dataRate);
	label = gtk_label_new (tmpBuffer);
     	gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 1, 2, 10, 11);
	gtk_widget_show (label);

	memset(tmpBuffer,'\0',30);
	sprintf(tmpBuffer,"%d",frmt);
	label = gtk_label_new (tmpBuffer);
     	gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 1, 2, 11, 12);
	gtk_widget_show (label);

	memset(tmpBuffer,'\0',30);
	sprintf(tmpBuffer,"%d",cfgcnt);
	label = gtk_label_new (tmpBuffer);
     	gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 1, 2, 12, 13);
	gtk_widget_show (label);

	memset(tmpBuffer,'\0',30);
	sprintf(tmpBuffer,"%d",cfg2_frm_size);
	label = gtk_label_new (tmpBuffer);
     	gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 1, 2, 13, 14);
	gtk_widget_show (label);

	label = gtk_label_new (" ");
	markup = g_markup_printf_escaped ("<span foreground=\"#333333\" font='8'><b>Note : Configuration and Server info of running PMU.</b></span>");
	gtk_label_set_markup (GTK_LABEL (label), markup);
     	gtk_misc_set_alignment (GTK_MISC(label),0,0);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 2, 15, 16);
	gtk_widget_show (label);
	g_free (markup);

	/* Signal handling for OK-button on dialog Window */
	g_signal_connect_swapped (close_but, "clicked", G_CALLBACK (gtk_widget_destroy), new_window);

	/* This makes it so the button is the default */
	gtk_widget_set_can_default (close_but, TRUE);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (new_window)->action_area), close_but, TRUE, TRUE, 0);

	/* This grabs this button to be the default button. Simply hitting the "Enter" key will cause this button to activate. */
	gtk_widget_grab_default (close_but);
	gtk_widget_show (close_but);

	/* Finally show the PMU Detailed window or new_window */
	gtk_widget_show (new_window);
};

/**************************************** End of File *******************************************************/
