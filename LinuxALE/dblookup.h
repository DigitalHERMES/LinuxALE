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
 *   dblookup.h - headers for callsign lookup
 * 
 * Version:
 *   $Revision$
 * 
 * Date:
 *   $Date$
 * 
 * Author:
 *   Glenn Valenta
 * 
 * History:
 *   $Log$
 *
 */

/*
DB FILE DISC

<3 letter callsign>|<50 char base name and local>|<100 char base URL>|<50 char webmap URL>\NL

empty strings are allowed but no blank spaces between deliminators.( delim )
there must not be any blank lines after the last entry in the table. In fact,
it would be a good practice to insert the lines in alphabetical and numerical
order to eliminate replications and the problem of blank lines on the end of 
the file.

*/


#ifndef __DBLOOKUP_H__
#define __DBLOOKUP_H__

#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdio.h>


#define DB_FILE_NAME "ale_basenames.db"
#define RECORD_DELIM "|"
#define RECORD_SIZE 205    /* add up all items in Station_record */
#define DISPLAY_INTERVAL 8 /* how often a lookup will repeat on screen */

/* Function Definitions */
void close_db();
char* search_db(char *);
int db_init();

typedef struct station_record {
  char db_station_id[4];       /* three letter callsign */
  char db_station_name[50];    /* Human readable name and location */
  char db_station_webURL[100]; /* URL for base (GUI Client) */
  char db_station_mapURL[50];  /* URL to web map of location */
  int hits;                    /* running tally of how many hits */
  int screen_age;              /* how long I'm on the screen */
  struct station_record *next_record_ptr;
} Station_record;
 


#endif

