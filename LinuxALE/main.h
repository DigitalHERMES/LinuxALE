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
 *   main.h - header file for Main module for LinuxALE
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
 *   Revision 1.1.1.1  2001/05/23 20:19:50  pile
 *   Initial version for sourceforge.net
 *
 *   Revision 0.2  2001/05/21 13:51:16  pile
 *   Minor fixes in output.
 *
 *   Revision 0.1  2001/05/20 19:43:25  pile
 *   Added text logging to file.
 *
 *   Revision 0.0.1.1  2001/05/17 20:25:33  pile
 *   LinuxALE
 *
 */

#ifndef __MAIN_H__
#define __MAIN_H__

#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/soundcard.h>
#include <math.h>
#include <stdio.h>
#include "time.h"
#include "server.h"

#define FRAME_SIZE  200
#define SAMPLE_RATE 8000

#if 0
/*
Global command line struct MAIN DEFINITION

This struct should be declared here but for some reason, the
compiler complained. It is declared in server.h but someday
I'll make it work here.

typedef struct { 
unsigned char silent;       /* disable terminal output */
FILE *write_file_fd;        /* write file pointer */
unsigned char write_server; /* enable server */
int port_num;               /* port num for server */ 
int interface_num;          /* ethernet interface to bind too */
int soundcard_fd;           /* future soundcard to bind to for input */
} command_line_options; */
#endif
 
/* ---------------------------------------------------------------------- */
static const char usage_str[] = "LinuxALE version 0.0.2 \n"
"Demodulates MIL-STD 188-141B ALE (Automatic Link Establishment) mode\n"
"(C) 2000, 2001 Charles Brain, Ilkka Toivanen, Glenn Valenta\n"
"  -f <filename> : decode from input file 16-bit 8000 Hz wav\n"
"  -s            : decode from sound card\n"
"  -l <filename> : write log file\n"
"  -v            : silent - no terminal\n"
"  -g            : enable server function\n";


/* ---------------------------------------------------------------------- */


/* Function Declerations */
/* void schedular(int); */

#endif
