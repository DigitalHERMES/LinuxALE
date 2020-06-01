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
 *   dblookup.c - functionality for callsign lookup
 * 
 * Version:
 *   $Revision$
 * 
 * Date:
 *   $Date$
 * 
 * Author:
 *   Glenn Valenta
 *   Ilkka Toivanen
 * 
 * History:
 *   $Log$
 *
 */

/*

dblookup.c

A lookup system to take the three letter call signs from the THRU, TO], 
TWS, FROM, and TIS preambles and look them up in a small database read
into memory from a file. The file also contains a place to put a web
URL for the base and a URL for a map lookup website for future 
expansion.

The file format is described and adjustable in the dblookup.h file.

Vambo A.K.A Glenn Valenta  glenn@coloradostudios.com

Wed Jun 13 00:21:33 MDT 2001

Things to do:

1) Add more stations to the database

2) Add websites to the database

3) Provide an function to retrieve total hits for a CS

4) add logging for unIDed CSs.

*/


#include "dblookup.h"


Station_record *station_table;
Station_record *screen_live[DISPLAY_INTERVAL+1];

/*** Read and init table of db into memory  */
int db_init()
{
  int i=0;                
  char S[RECORD_SIZE];      
  char *s0,*s1,*s2,*s3,*s4;
  FILE *file = NULL;
  Station_record *record_ptr, *last_record_ptr;
  
  /* null out pointer table */
  for(i=DISPLAY_INTERVAL;i>=0;--i)
    screen_live[i]=NULL;
      
  /* printf("screen_live init\n"); */

  file = fopen(DB_FILE_NAME, "r");
  if(file == NULL)
    {  
      printf("Can't open %sn","DB_FILE_NAME");
      return(1);
    }

  i=0;
  while (!feof(file)) { 
    /* get line from file */
    s0=fgets(S,RECORD_SIZE,file);
    if (s0) {   
      
      /* get place to put it */
      record_ptr = (Station_record *)malloc( sizeof( Station_record ) );
      if( record_ptr == NULL )
	{  
	  printf("Can't malloc memory\n");
	  return(3);
	}
      
      /* divide string into substrings */
      s1 = strsep( &s0, RECORD_DELIM );
      s2 = strsep( &s0, RECORD_DELIM );
      s3 = strsep( &s0, RECORD_DELIM );
      s4 = strsep( &s0, RECORD_DELIM );
      
      /* copy substrings into new struct */
      strcpy(record_ptr->db_station_id,s1);
      strcpy(record_ptr->db_station_name,s2);
      strcpy(record_ptr->db_station_webURL,s3);
      strcpy(record_ptr->db_station_mapURL,s4);
      record_ptr->hits=0;
      record_ptr->screen_age=0;
      
      /* if first one, record start of table */
      if(i==0) {
	station_table=record_ptr;
      }
      /* else point last record to this record to link the list */
      else {
	last_record_ptr->next_record_ptr = record_ptr;
      }
      
      /* make this record the previous for next iteration */
      last_record_ptr = record_ptr;
      i++;
      
    }
  }
  fclose(file);
  return(0);
}


/*  Looks up and appends the station name to three letter CS  */
char* search_db(char *msg)
{
  int counter=DISPLAY_INTERVAL;
  /* start at begining of table */
  Station_record *record_ptr=station_table;  

  /* go back three chars from end of string */
  char *cs_ptr = msg+(char)strlen(msg)-3;  

  /*  printf("searching"); */
 
  /* age the screen life */
  for( counter = DISPLAY_INTERVAL; counter >= 1; --counter )
    screen_live[ counter ] = screen_live[ counter - 1 ];

  screen_live[ 0 ] = NULL;
 
  /* loop till end of list */
  while( record_ptr->next_record_ptr != NULL )
    {
      /* printf("."); */       /* if CS matches a table entry */
      if( ! strncmp( cs_ptr, record_ptr->db_station_id, 3 ) )
	{
	  record_ptr->hits++;   /* inc the hits */

	  /* check to see if it's displayed already */
	  for( counter = 0; counter <= DISPLAY_INTERVAL ; counter++ )
	    {                   
	      if( screen_live[counter] == record_ptr )
		return( msg );    /* if it's on the display already, return */
	    } 
	  screen_live[0] = record_ptr;  
	  /*  if not, append the station lookup string */
	  strcat( msg, " " );
	  strcat( msg, record_ptr->db_station_name );
	  return( msg ); 
	}
	
      record_ptr = record_ptr->next_record_ptr;
    }
  return( msg );  /* return if nothing found in lookup */
}





void close_db()
{
  free(station_table);  /* give back all of the memories */
}

/*
int main(int argc, char *argv[])
{

  char s10[100],s20[100],s30[100];
  sprintf (s10, "%s","[02:57:57] [TWS] JNR");
  sprintf (s20, "%s","[03:12:27] [TWS] PLA");
  sprintf (s30, "%s","[03:19:36] [TO] OFF");

  db_init();
  printf("Start search\n");

  printf("%s\n",search_db(s10));
 sprintf (s10, "%s","[02:57:57] [TWS] JNR");
   printf("%s\n",search_db(s10));
 sprintf (s10, "%s","[02:57:57] [TWS] JNR");
  printf("%s\n",search_db(s10));
 sprintf (s10, "%s","[02:57:57] [TWS] JNR");
  printf("%s\n",search_db(s10));
 sprintf (s10, "%s","[02:57:57] [TWS] JNR");
  printf("%s\n",search_db(s10));
 sprintf (s10, "%s","[02:57:57] [TWS] JNR");
 printf("%s\n",search_db(s10));
 sprintf (s10, "%s","[02:57:57] [TWS] JNR");
  printf("%s\n",search_db(s10));

  printf("%s\n",search_db(s10));
 sprintf (s10, "%s","[02:57:57] [TWS] JNR");
   printf("%s\n",search_db(s10));
 sprintf (s10, "%s","[02:57:57] [TWS] JNR");
  printf("%s\n",search_db(s10));
 sprintf (s10, "%s","[02:57:57] [TWS] JNR");
  printf("%s\n",search_db(s10));
 sprintf (s10, "%s","[02:57:57] [TWS] JNR");
  printf("%s\n",search_db(s10));
 sprintf (s10, "%s","[02:57:57] [TWS] JNR");
 printf("%s\n",search_db(s10));
 sprintf (s10, "%s","[02:57:57] [TWS] JNR");
  printf("%s\n",search_db(s10));

  printf("%s\n",search_db(s10));
 sprintf (s10, "%s","[02:57:57] [TWS] JNR");
   printf("%s\n",search_db(s10));
 sprintf (s10, "%s","[02:57:57] [TWS] JNR");
  printf("%s\n",search_db(s10));
 sprintf (s10, "%s","[02:57:57] [TWS] JNR");
  printf("%s\n",search_db(s10));
 sprintf (s10, "%s","[02:57:57] [TWS] JNR");
  printf("%s\n",search_db(s10));
 sprintf (s10, "%s","[02:57:57] [TWS] JNR");
 printf("%s\n",search_db(s10));
 sprintf (s10, "%s","[02:57:57] [TWS] JNR");
  printf("%s\n",search_db(s10));





  printf("%s\n",search_db(s20));
  printf("%s\n",search_db(s30));

  close_db();
}
*/
