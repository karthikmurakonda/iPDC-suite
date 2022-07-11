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
/*                  global Data Structure for Shared Memory           */
/* ------------------------------------------------------------------ */

struct P_id {

	pid_t pid;               	/* Single pid */

	int dataFileVar;		/* For data source options : 0-Auto & 1-File Measurements */
	char filePath[400];		/* File path of user selected measurement file */
	int UdpPort;			/* User given UDP port number for server */
	int TcpPort;			/* User given TCP port number for server */
	int cfg_bit_change_info;	/* STAT Word change bit status if any otherwise default set to 0 */
	char cfgPath[200];       	/* Its a PMU Setup File path given by use */
	int phasor_type[50];		/* save the phasor type (currently not useful) */

}p1, *ShmPTR;

/**************************************** End of File *******************************************************/
