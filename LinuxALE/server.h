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
 *   server.h - header file for server.c
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
 *   Revision 1.1  2001/06/17 19:39:35  pile
 *   Socket server functionality by GV
 *
 *
 */


#ifndef __SERVER_H__
#define __SERVER_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>


#define MAX_CONNECTIONS 10
#define SERVER_GREETING_MESSAGE "Welcome To LinuxALE server Ver 0.0.2 \n"
#define SERVER_FULL_MESSAGE "SORRY! Server Is Full - LinuxALE server Ver 0.0.2 \n"

/***** Real declaration in main.c   ***/
/*
This struct should not be declared here but for some reason, the
compiler complained when it was placed in main where it should be.
It is declared here ATM but someday I'll make it work from main.h
*/
typedef struct{  
  unsigned char silent;       
  unsigned char cs_enable;
  FILE *write_file_fd;           
  unsigned char write_server; 
  int port_num;              
  int interface_num;          
  int soundcard_fd;          
} command_line_options; 

/* remeber, this is instanciated in main.c */
extern command_line_options Command_line_options;

/* Function Definitions */

void nullfunc(int);
void *client_handler(void*);
void send_server(char*);
void *server_init(void *arg);

/* client thread information */
typedef struct{
  int _pid;
  int _socket;
} client;


#endif 
