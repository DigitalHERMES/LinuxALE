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
 *   main.c - Main module for ALE decoder
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

#include "main.h"


/* Global */
command_line_options Command_line_options;

void schedular(int fd, FILE *log_file)
{
  unsigned char sout[FRAME_SIZE*2];
  int readstat;

  modem_init();

  while(1)
    {
      if (readstat = read(fd, sout, FRAME_SIZE*2) > 0)
        {
          modem((unsigned short*)sout,FRAME_SIZE, log_file);
        }
      else {
        exit (-1);
      }
    }
}

int main(int argc, char *argv[])
{
  int i, c, errflg = 0;
  int fd;                        /* file descriptor of input file */
  int arg;
  int sndparam;                  /* parameter to use when initializing 
              			    the sound card */
  int fmt;
  int sample_rate = SAMPLE_RATE; /* sample rate from audio card */
  FILE *log_file = NULL;         /* pointer to log file */


  pthread_t a_thread;          /*  for server */

  Command_line_options.silent=0;     /*  set some defaults */
  Command_line_options.write_file_fd=NULL;
  Command_line_options.write_server=0;
  Command_line_options.port_num=47047;
  Command_line_options.interface_num=0;
  Command_line_options.soundcard_fd=0;
  Command_line_options.cs_enable = 0; 


  if (argc < 2) 
    {
      (void)fprintf(stderr, usage_str);
      exit(2);
    }

  /* Read the parameters from the command line */
  while ((c = getopt(argc, argv, "l:h::f:s::g::v::i::")) != EOF) 
    {
      switch (c)
	{
	case '?':
	case 'h':
	  errflg++;
	  break;
        case 'l':
          /* Open log file */
	    log_file = fopen(optarg, "a");
	    if (!log_file) {
            perror ("open log file");
            exit(10);
          }
	break;
	case 'f':
	  /* Open file */
	  if ((fd = open(optarg, O_RDONLY)) < 0) {
	    perror("open");
	    exit(10);
	  }
	  break;
	 
	case 'g': 
	  /* Enable server */
	  {
	    Command_line_options.write_server=1;
	    pthread_create(&a_thread,NULL,server_init,(void*)NULL);
	  }
	  break;

        case 'v': 
	  /* Disable terminal output (silent to terminal) */
	  Command_line_options.silent=1;
	  break;
	  
	case 'i': 
	  /* enable callsign lookup */
	  Command_line_options.cs_enable = 1;
	  db_init();
	  break;

	case 's':
	  /* Open and initialize the sound card */
	  if ((fd = open("/dev/dsp", O_RDONLY)) < 0) {
	    perror("open");
	    exit (10);
	  }

  
	  sndparam = AFMT_S16_LE; /* we want 16 bits/sample signed */
	  /* little endian; works only on little endian systems! */
	  if (ioctl(fd, SNDCTL_DSP_SETFMT, &sndparam) == -1) {
	    perror("ioctl: SNDCTL_DSP_SETFMT");
	    exit (10);
	  }
	  if (sndparam != AFMT_S16_LE) {
	    fmt = 1;
	    sndparam = AFMT_U8;
	    if (ioctl(fd, SNDCTL_DSP_SETFMT, &sndparam) == -1) {
	      perror("ioctl: SNDCTL_DSP_SETFMT");
	      exit (10);
	    }
	    if (sndparam != AFMT_U8) {
	      perror("ioctl: SNDCTL_DSP_SETFMT");
	      exit (10);
	    }
	  }
	  sndparam = 0;   /* we want only 1 channel */
	  if (ioctl(fd, SNDCTL_DSP_STEREO, &sndparam) == -1) {
	    perror("ioctl: SNDCTL_DSP_STEREO");
	    exit (10);
	  }
	  if (sndparam != 0) {
	    fprintf(stderr, "soundif: Error, cannot set the channel "
		    "number to 1\n");
	    exit (10);
	  }
	  sndparam = sample_rate; 
	  if (ioctl(fd, SNDCTL_DSP_SPEED, &sndparam) == -1) {
	    perror("ioctl: SNDCTL_DSP_SPEED");
	    exit (10);
	  }
	  if ((10*abs(sndparam-sample_rate)) > sample_rate) {
	    perror("ioctl: SNDCTL_DSP_SPEED");
	    exit (10);
	  }
	  if (sndparam != sample_rate) {
	    fprintf(stderr, "Warning: Sampling rate is %u, "
		    "requested %u\n", sndparam, sample_rate);
	  }
	  
	  
	  break;
	default:
	  errflg++;
	  break;
	  
	}
    }

  if (errflg) {
    (void)fprintf(stderr, usage_str);
    exit(2);
  }

  schedular (fd, log_file);
  return 0;
}

