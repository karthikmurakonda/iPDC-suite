/* ----------------------------------------------------------------------------- 
 * function.c
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


/* -------------------------------------------------------------------------------------- */
/*                            Functions defined in function.c                             */
/* -------------------------------------------------------------------------------------- */


/* ---------------------------------------------------------------------------- */
/*                                                                              */
/*   1. void B_copy(unsigned char main[],unsigned char tmp[],int ind,int n)	*/
/*   2. char* measurement_Return ()             	       			          */
/*   3. void H2S(char a[], unsigned char temp_6[])  	  	    		          */
/*   4. void i2c (int t, unsigned char temp[])	  			               */
/*   5. void li2c (long int t1, unsigned char temp_1[])    			          */
/*   6. void f2c (float f, unsigned char temp_4[])	 			          */
/*   7. int c2i (unsigned char temp_2[])		 			               */
/*   8. long int c2li (unsigned char temp_3[])				               */
/*   9. uint16_t compute_CRC(unsigned char *message,char length)		          */
/*   10.void sigchld_handler(int s)			 			               */
/*                                                                              */
/* ---------------------------------------------------------------------------- */

#include  <stdio.h>
#include  <string.h>
#include  <stdlib.h>
#include  <stdint.h>
#include  <dirent.h>
#include  <sys/wait.h>
#include  "function.h"
#include  "ServerFunction.h"

#define channel_name 100


/* ----------------------------------------------------------------------------	*/
/* FUNCTION B_copy(unsigned char main[], unsigned char tmp[], int ind, int n):	*/
/* Function copies unsigned char or Bytes in a main array from tmp array.       */
/* ----------------------------------------------------------------------------	*/

void B_copy(unsigned char main[], unsigned char tmp[], int ind, int n)
{
	int k;
	for(k=0; k<n; k++)
	{
		main[ind+k] = tmp[k];
	}
};


/* ----------------------------------------------------------------------------	*/
/* FUNCTION char* measurement_Return ():	               				*/
/* Function to read the measurement file and return all measurements values for */
/* a singel timestamp.								                    */
/* ----------------------------------------------------------------------------	*/

char* measurement_Return ()
{
	char *l1;
	size_t l2 = 0;
	ssize_t result;

	while(1)
	{
		while ((result = getdelim (&l1, &l2, ('\n'), fp_DataFile)) >0)
		{
			return l1;
		}
		fseek(fp_DataFile, 0, SEEK_SET);	
		result = 1;
	}
	fclose(fp_DataFile);
}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION H2S(char a[], unsigned char temp_6[]):          				*/
/* Function for unsigned/Hexa char to String Conversion.   					*/
/* ----------------------------------------------------------------------------	*/

void H2S(char a[], unsigned char temp_6[])
{
	int k;

	for(k=0; k<16; k++)
	{
		a[k] = temp_6[k];
	}
	a[16] = '\0';
}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION i2c (int t, unsigned char temp[]):         					*/
/* Function for Integer to unsigned Character Conversion        				*/
/* ----------------------------------------------------------------------------	*/

void i2c (int t, unsigned char temp[])
{
	temp[0] = t>>8;
	temp[1] = t;
}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION li2c (long int t1, unsigned char temp_1[]):	     			*/
/* Function for Long Integer to unsigned Character Conversion.  				*/
/* ----------------------------------------------------------------------------	*/

void li2c (long int t1, unsigned char temp_1[])
{
	temp_1[0] = t1>>24;
	temp_1[1] = t1>>16;
	temp_1[2] = t1>>8;
	temp_1[3] = t1;
}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION li2c (long int t1, unsigned char temp_1[]):	     			*/
/* Function for float to unsigned Character Conversion		     			*/
/* ----------------------------------------------------------------------------	*/

void f2c (float f, unsigned char temp_1[])
{
	int i, j;
	float fv;
	unsigned char a1[sizeof fv];

	fv = f;
	memcpy(a1, &fv, sizeof fv);
	for (i=0, j=3; i<sizeof fv; i++, j--)
	{
		temp_1[j] = a1[i];
	}
}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION c2i (unsigned char temp[]):					               	*/
/* Function for unsigned Character to Integer Conversion					*/
/* ----------------------------------------------------------------------------	*/

int c2i (unsigned char temp[])
{
	int i;

	i = temp[0];
	i<<=8;
	i |=temp[1];
	return(i);
}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION c2li (unsigned char temp_3[]):					               */
/* Function for unsigned Character to Long Integer Conversion 				*/
/* ----------------------------------------------------------------------------	*/

long int c2li (unsigned char temp_3[])
{
	long int i;
	unsigned char a[4];
	memset(a, '\0', 4);
	strcpy((char *)a, (char *)temp_3);

	i = a[0];
	i<<=8;
	i |=a[1];
	i<<=8;
	i |=a[2];
	i<<=8;
	i |=a[3];
	return(i);
}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION uint16_t compute_CRC(unsigned char *message,char length):		     */
/* Function for calculation of CHECKSUM CRC-CCITT (0xffff) of string of         */
/* unsigned Character.                                                          */
/* ----------------------------------------------------------------------------	*/

uint16_t compute_CRC(unsigned char *message,int length)
{
	uint16_t crc=0x0ffff,temp,quick;
	int i;

	for(i=0;i<length;i++)
	{
		temp=(crc>>8)^message[i];
		crc<<=8;
		quick=temp ^ ( temp >>4);
		crc ^=quick;
		quick<<=5;
		crc ^=quick;
		quick <<=7;
		crc ^= quick;
	}
	return crc;
}


/* ----------------------------------------------------------------------------	*/
/* FUNCTION sigchld_handler(int s):						               */
/* Function for TCP connection signal handling					          */
/* ----------------------------------------------------------------------------	*/

void sigchld_handler(int s)
{
	while(wait(NULL) > 0);
}

/**************************************** End of File *******************************************************/
