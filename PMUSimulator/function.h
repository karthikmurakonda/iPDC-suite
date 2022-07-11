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


/* ------------------------------------------------------------------ */
/*                       Function prototypes                          */
/* ------------------------------------------------------------------ */

/* Function copies Bytes in a main array */ 
void B_copy(unsigned char main[], unsigned char tmp[], int ind, int n);

/* Function to read the measurement file and return measurements for a singel timestamp */ 
char* measurement_Return ();

/* Function for Hexa to String Conversion */ 
void H2S(char a[], unsigned char temp_6[]);

/* Function for Integer to Character Conversion */ 
void i2c (int t, unsigned char temp[]);

/* Function for Long Integer to Character Conversion */ 
void li2c (long int t1, unsigned char temp_1[]);

/* Function for float to Character Conversion */ 
void f2c (float f, unsigned char temp_4[]);

/* Function for Character to Integer Conversion */ 
int c2i (unsigned char temp_2[]);

/* Function for Character to Long Integer Conversion */ 
long int c2li (unsigned char temp_3[]);

/* Function for calculation of CHECKSUM CRC-CCITT (0xffff) */
uint16_t compute_CRC(unsigned char *message,int length);

/* Function for TCP connection signal handling */ 
void sigchld_handler(int s);

/**************************************** End of File *******************************************************/
