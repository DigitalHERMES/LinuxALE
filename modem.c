/* -*- c -*-
 * 
 * Copyright (C) 2000 - 2001
 *   Charles Brain (chbrain@dircon.co.uk)
 *   Ilkka Toivanen (pile@aimo.kareltek.fi)
 *   Glenn Valenta (glenn@coloradostudios.com)
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * File:
 *   modem.c  FFT and decoder functions for LinuxALE
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
 *   Revision 1.3  2001/06/17 19:39:35  pile
 *   Socket server functionality by GV
 *
 *   Revision 1.2  2001/05/29 18:53:44  pile
 *   Fixed sync problems and cleaned up garbage printing (decode_word).
 *
 *   Revision 1.1.1.1  2001/05/23 20:19:50  pile
 *   Initial version for sourceforge.net
 *
 *   Revision 0.3  2001/05/21 13:51:16  pile
 *   Minor fixes in output.
 *
 *   Revision 0.2  2001/05/20 20:41:10  pile
 *   Fixed \n and DATA output.
 *
 *   Revision 0.1  2001/05/20 19:42:52  pile
 *   Added text logging to file.
 *
 *   Revision 0.0.1.1  2001/05/20 06:10:06  pile
 *   LinuxALE
 *
 */

#include "modem.h"

/* consolidate all output to one place */
void output_mesg(char *mesg, FILE *log_file)
{
      if(!Command_line_options.silent)
	printf ("%s\n", mesg);

      if(Command_line_options.write_server)
	send_server(mesg); 

      if(log_file)
	{
	  fprintf(log_file,"%s\n", mesg);
	  fflush(log_file);
	}
}


void decodeCMD (unsigned long word) 
{
  int firstletter, metacmd;

  firstletter = (word>>14)&0x0F;
  metacmd = (word>>14)&0x70;

  if (metacmd == 6)
    {
      printf (" %s ", CMD_types[firstletter]);
      /* this part is still under development.
      fprintf (stdout, " %s", CMD_types[firstletter]);
      if (log_file) {
	fprintf (log_file, " %s", CMD_types[firstletter]);
      }
      */
    }
}

void decode_word (unsigned long word, FILE *log_file)
{
  unsigned char a, b, c, preamble;
  static int started; /* if other than DATA has arrived */
  time_t timestamp;
  struct tm *UTC_time;

  char msg[200];
  char tmpBuffer[200];
  memset(msg, '\0', 200);
  memset(tmpBuffer, '\0', 200);
  
  timestamp = time(NULL);
  UTC_time=gmtime(&timestamp);
  sprintf (msg, "[%02d:%02d:%02d] ", UTC_time->tm_hour,
	   UTC_time->tm_min, UTC_time->tm_sec);

  c = word&0x7F;
  b = (word>>7)&0x7F;
  a = (word>>14)&0x7F;
  preamble = (word>>21)&0x7;

  /* if CMD just print the stuff */
  if (preamble == 6) 
    {
      sprintf (tmpBuffer, "%s ",preamble_types[preamble]);
      strcat (msg, tmpBuffer);
      sprintf(tmpBuffer, "%c%c%c",a,b,c);
      strcat (msg, tmpBuffer);

      output_mesg(msg, log_file);

    }
  else 
    /* if other than CMD */
    {
      /* check if proper charset i.e. proper ale addresses */
      if((ASCII_Set[a]==ASCII_38)&&(ASCII_Set[b]==ASCII_38)&&(ASCII_Set[c]==ASCII_38)) 
	{
	  sprintf (tmpBuffer, "%s ",preamble_types[preamble]);
	  strcat (msg, tmpBuffer);
	  sprintf(tmpBuffer, "%c%c%c",a,b,c);
	  strcat (msg, tmpBuffer);

          if(Command_line_options.cs_enable) {
            if(preamble>=1&&preamble<=5) {
              search_db(msg);
	    }
	  }
	  output_mesg(msg, log_file);
	}
    }
}

unsigned long modem_de_interleave_and_fec(int *input, int *errors)
{
  int i;
  unsigned long worda;
  unsigned long wordb;
  unsigned int error_a, error_b;
  
  worda = wordb = 0;
	
  for(i = 0; i < VOTE_BUFFER_LENGTH; )
    {
      worda = input[i++] ? (worda<<1)+1 : worda<<1 ;
      wordb = input[i++] ? (wordb<<1)+1 : wordb<<1 ; 
    }
  wordb = wordb ^ 0x000FFF;
  /*
    Now do the FEC
  */
  worda = golay_decode(worda,&error_a);
  wordb = golay_decode(wordb,&error_b);
  /*
    Return the number of errors as the word with the most.
  */
  if(error_a > error_b) 
    *errors = error_a;
  else
    *errors = error_b;
  /*
    Format into a single 24 bit word
  */ 
  worda = (worda<<12) + wordb;
  return (worda);
}
/*
  Process a new received symbol
*/
void modem_new_symbol(int sym, FILE *log_file)
{
  static int 		bits[VOTE_ARRAY_LENGTH];
  static int 		input_buffer_pos;
  static int 		word_sync_position;
  int        		majority_vote_array[VOTE_BUFFER_LENGTH];
  int        		bad_votes, sum, errors, i;
  unsigned long         word;


  /*
    Update the input buffer
  */
  bits[input_buffer_pos] = (sym&4) ? 1 : 0;
  input_buffer_pos = ++input_buffer_pos%VOTE_ARRAY_LENGTH;
  bits[input_buffer_pos] = (sym&2) ? 1 : 0;
  input_buffer_pos = ++input_buffer_pos%VOTE_ARRAY_LENGTH;
  bits[input_buffer_pos] = (sym&1) ? 1 : 0;
  input_buffer_pos = ++input_buffer_pos%VOTE_ARRAY_LENGTH;
  /*
    Do the majority vote, ignore stuff bits
  */
  bad_votes = 0;
  for(i=0; i<VOTE_BUFFER_LENGTH; i++)
    {
      sum  = bits[(i+input_buffer_pos)%VOTE_ARRAY_LENGTH];
      sum += bits[(i+input_buffer_pos+SYMBOLS_PER_WORD)%VOTE_ARRAY_LENGTH];
      sum += bits[(i+input_buffer_pos+(2*SYMBOLS_PER_WORD))%VOTE_ARRAY_LENGTH];
      if(( sum == 1 ) || ( sum == 2)) bad_votes++; 
      majority_vote_array[i] = vote_lookup[sum];
    }
  /*    
    printf("Votes %d\n",bad_votes);

    Now do tests to see if it might be time for a word sync
  */
  if( word_sync == NOT_WORD_SYNC )
    {
      if( bad_votes <= BAD_VOTE_THRESHOLD )
	{
	  word = modem_de_interleave_and_fec(majority_vote_array,&errors);
	  
	  if(errors <= SYNC_ERROR_THRESHOLD)
	    {
	      /*
		Word sync may have occurred
		Make sure BASIC alphabet characters have been received to confirm
	      */ 
	      
	      decode_word(word, log_file);
	      /* now we have a proper sync; added after 0.0.1 */
	      word_sync = WORD_SYNC;
	      word_sync_position = input_buffer_pos; 

	    }
	}
    }
  else
    {
      if( input_buffer_pos == word_sync_position )
	{
	  
	  /* Signal new word */
	  word = modem_de_interleave_and_fec(majority_vote_array,&errors);
	  decode_word (word, log_file);
	}
      else 
	{
	  /* we don't have a proper sync; added after 0.0.1 */
	  word_sync = NOT_WORD_SYNC;
	}
    }			
}
void modem_init(void)
{
  int i;
  
  /*
    Create the twiddles etc for the FFT
  */
  for(i=0; i<FFT_SIZE; i++)
    {
      fft_cs_twiddle[i] = cos(-2.0*PI*i/FFT_SIZE);
      fft_ss_twiddle[i] = sin(-2.0*PI*i/FFT_SIZE);
      fft_history[i]    = 0;
    }
  fft_history_offset = 0;
  
  word_sync = NOT_WORD_SYNC;
}
/*
  Main Signal processing routine
*/
void modem(unsigned short *sample, int length, FILE *log_file)
{
  int i,n,max_offset;
  double new_sample;
  double old_sample;
  double temp_real;
  double temp_imag;
  double max_magnitude;
  static int last_symbol;
  static int last_sync_position;
  static int sample_count;
  
  for(i=0; i<length; i++)
    {
      if(sample[i]&0x8000) 
	/*
	  converting between 2's compliment and floating point maths 
  	  a mystical factor is (1/2^15)
	*/
	new_sample = -(((~sample[i])&0x7FFF)*0.0000305176);
      else
	new_sample = sample[i]*0.0000305176;
      /*
	Do the Sliding FFT
      */
      old_sample = fft_history[fft_history_offset];
      fft_history[fft_history_offset] = new_sample;
      /*
	Update output, only do first half, imag in = 0;
      */
      for(n=0; n<FFT_SIZE/2; n++)
	{
	  temp_real       = fft_out[n].real-old_sample+new_sample;
	  temp_imag       = fft_out[n].imag;
	  fft_out[n].real = (temp_real*fft_cs_twiddle[n]) - (temp_imag*fft_ss_twiddle[n]);
	  fft_out[n].imag = (temp_real*fft_ss_twiddle[n]) + (temp_imag*fft_cs_twiddle[n]);
	  fft_mag[n]      = sqrt((fft_out[n].real*fft_out[n].real)+(fft_out[n].imag*fft_out[n].imag));


	}
      /*
	Save the max output value in a running average over one ALE word
	This is used for bit sync and symbol decoding
      */ 
      max_magnitude = 0;
      max_offset    = 0;
      /*      for( n = 6; n <= 22; n += 2)*/
      for( n = 6; n <= 22; n++)
	{
	  if( fft_mag[n] > max_magnitude )
	    {
	      max_magnitude = fft_mag[n];
	      max_offset    = n;
	    }
	} 
      /*
	Update the magnitude history for sync purposes
      */
      mag_sum[sample_count]                        -= mag_history[sample_count][mag_history_offset];
      mag_sum[sample_count]                        += max_magnitude;		
      mag_history[sample_count][mag_history_offset] = max_magnitude;

      
      /*
	Decide what to do 
      */
      if(word_sync == NOT_WORD_SYNC)
	{
	  /*
	    Update bit sync position
	  */
	  max_magnitude = 0;
	  for( n=0; n < FFT_SIZE; n++)
	    {
	      if(mag_sum[n] > max_magnitude)
		{
		  max_magnitude = mag_sum[n];
		  last_sync_position = n;
		}
	    }
	    last_symbol = symbol_lookup[max_offset];    // HZ added March 8, 2021
	}



      if( sample_count == last_sync_position)
	{
	  /* 
	     See what symbol has been received
	  */

	  last_symbol = symbol_lookup[max_offset]; 
	}
      /*
	Always output at the same position
      */
      if( sample_count == 0 )
	{
	  modem_new_symbol(last_symbol, log_file);
	}
      fft_history_offset = ++fft_history_offset%FFT_SIZE;
      sample_count       = ++sample_count%MOD_64; 
      if(sample_count == 0)
	mag_history_offset = ++mag_history_offset%SYMBOLS_PER_WORD;
    }
}
















