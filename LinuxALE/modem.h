/* -*- c -*-
 * 
 * Copyright (C) 2000 - 2001 
 *   Charles Brain (chbrain@dircon.co.uk)
 *   Ilkka Toivanen (pile@aimo.kareltek.fi)
 *   Glenn Valenta (glenn@coloradostudios.com)
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
 *   modem.h - Modem.c main file headers
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
 *   Glenn Valenta
 * 
 * History:
 *   $Log$
 *   Revision 1.2  2001/06/17 19:39:35  pile
 *   Socket server functionality by GV
 *
 *   Revision 1.1.1.1  2001/05/23 20:19:50  pile
 *   Initial version for sourceforge.net
 *
 *   Revision 0.1  2001/05/20 19:43:57  pile
 *   Added text logging to file.
 *
 *   Revision 0.0.1.1  2001/05/17 07:10:57  pile
 *   LinuxALE
 *
 */


#ifndef __MODEM_H__
#define __MODEM_H__


#include "math.h"
#include "golay.h"
#include "server.h"
#include "dblookup.h"
#include <time.h>
#include <stdio.h>

#define FFT_SIZE                    64
#define MOD_64                      64
#define SYMBOLS_PER_WORD            49
#define VOTE_BUFFER_LENGTH          48
#define NOT_WORD_SYNC               0
#define WORD_SYNC                   1
#define BITS_PER_SYMBOL             3
#define VOTE_ARRAY_LENGTH           (SYMBOLS_PER_WORD*BITS_PER_SYMBOL)
#define PI                          M_PI
#define BAD_VOTE_THRESHOLD          25
#define SYNC_ERROR_THRESHOLD        1

#define ASCII_128  0
#define ASCII_64   1
#define ASCII_38   2


/* Function Definitions*/

void output_mesg(char*, FILE*);
void decodeCMD (unsigned long);
void decode_word (unsigned long, FILE*);
void modem_new_symbol(int, FILE*);
unsigned long modem_de_interleave_and_fec(int*,int*);
void modem_init(void);
void modem(unsigned short*,int, FILE*);



/* Real declaration in main.c */
/* extern command_line_options Command_line_options; */

typedef struct{
  double real;
  double imag;
} Complex;

static const char *preamble_types[] = {
  "[DATA]", "[THRU]", "[TO]", "[TWS]", "[FROM]", "[TIS]", "[CMD]",
  "[REP]"
};

static const char *CMD_types[] = {
  "[Advanced LQA]", "[LQA]", "[DBA]", "[Channels]", "[DTM]", "[Freq]",
  "[Mode selection]", "[Noise report]"
};

char ASCII_Set[128] = {
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
         2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const int symbol_lookup[22] = {8,8,8,8,8,8,0,8,1,8,3,8,2,8,6,8,7,8,5,8,4,8};
const int vote_lookup[4]    = {0,0,1,1};	

/*
 * FFT information
 */
static double  fft_cs_twiddle[FFT_SIZE];
static double  fft_ss_twiddle[FFT_SIZE];
static double  fft_history[FFT_SIZE];
static Complex fft_out[FFT_SIZE];
static double  fft_mag[FFT_SIZE];
static int     fft_history_offset;

/*
 * sync information
 */
static double mag_sum[FFT_SIZE];
static double mag_history[FFT_SIZE][SYMBOLS_PER_WORD];
static int    mag_history_offset;
static int    bit_count;
static int    word_sync;


#endif 


