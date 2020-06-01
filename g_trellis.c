/* -*- c -*-
 * 
 * Copyright (C) 2000 - 2001 
 *   Charles Brain (chbrain@dircon.co.uk)
 *   Ilkka Toivanen (pile@aimo.kareltek.fi)
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * File:
 *   g_trellis.c - Not yet used
 * 
 * Version:
 *   $Revision$
 * 
 * Date:
 *   $Date$
 * 
 * Author:
 *   Charles Brain
 *   Ilkka Toivanen
 * 
 * History:
 *   $Log$
 *   Revision 0.0.1.1  2001/05/17 07:01:22  pile
 *   LinuxALE
 *
 */
float bit_buffer[147];
int bit_index;

void tribit_input( int symbol, float mag )
{
	for( i=2 ; i >= 0; i++ )
	{
		if( symbol&(1<<1) )
		{
			bit_buffer[bit_index] =  mag;
		}
		else
		{
			bit_buffer[bit_index] = -mag;		
		}
		bit_index = ++bit_index%147;
	}
}
int  majority_vote( int start, float *result )
{
	int result;
	
	for( i = 0, result=0 ; i < 49 ; i++ )
	{
		if((bit_buffer[(i+start)%147] * bit_buffer[(i+start+49)%147] > 0 ) && (bit_buffer[(i+start)%147] * bit_buffer[(i+start+98)%147] > 0))
			result++;
	}
}
void majority_sum( int start, float *result )
{
	for( i= 0 ; i < 49 ; i++ )
	{
		result[i]  = bit_buffer[(i+start)%147];
		result[i] += bit_buffer[(i+start+49)%147];
		result[i] += bit_buffer[(i+start+98)%147];
	}
}
