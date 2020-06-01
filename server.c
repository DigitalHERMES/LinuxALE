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
 *   server.c - adds server functionality to LinuxALE
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
 *   Revision 1.1  2001/06/17 19:39:35  pile
 *   Socket server functionality by GV
 *
 *
 */

/*
server.c

A server, created in it's own thread to listen and wait for client 
connections. 

When a connection is received, server_init() spawns a new thread in 
client_handler(). Client_handler() pauses and waits for calls from the
parent main() thread to send_server() to send a signal to each of the PIDs
in the client_list to wake them up to send the string to each client. Each
thread in client_handler checks to see if each connection is still go. If 
not, it cleans it's self up.

Vambo A.K.A Glenn Valenta  glenn@coloradostudios.com

Wed Jun  6 00:20:54 MDT 2001

Things to do:

1) a disconnected client isn't detected until the next ALE is received

2) user selectiable network interfaces

3) user selectable port number

4) Gracfully exit on ctrl-C

*/


#include "server.h"


/* Globals */
/* array of PIDs of all the client threads */
client client_list[MAX_CONNECTIONS];

/* global string read by each client thread */
char msg[50]; 

/* I need this stupid function to satisfy the signal function */
void nullfunc(int sig)
{ }




void *client_handler(void* pointer2myself)
{ 
  
  int _error;
  char greetings[]=SERVER_GREETING_MESSAGE;

  /* catch signal calls null function */
  (void)signal(SIGCONT,nullfunc);  

  /* send inital greeting to client before entering loop */
  _error=send(client_list[(int)pointer2myself]._socket,greetings,strlen(greetings),MSG_NOSIGNAL);
  
  if(_error==-1)  
    /* if client socket is already dead, close socket, kill thread */
    {    
      close(client_list[(int)pointer2myself]._socket);
      client_list[(int)pointer2myself]._pid=-1;
      pthread_exit(0);
    }
  
  while(1) {  /* keep looping sending new mesg to client */
    _error=send(client_list[(int)pointer2myself]._socket,msg,strlen(msg),MSG_NOSIGNAL);
    
    /* if client socket is dead, close socket, kill thread */
    if(_error==-1)  
      {
	close(client_list[(int)pointer2myself]._socket);
	client_list[(int)pointer2myself]._pid=-1;
	pthread_exit(0);
      }
    pause();            /* wait here till thread gets a signal */
  }  /* loop forever */
}




void send_server(char *omsg)
{           
  int _client_pointer;
  /* Puts mesg into global string */
  strcpy(msg,omsg);  
  /* add CR/NL */
  strcat (msg,"\n");
  
  /* then send signal to all the client threads */
  for(_client_pointer=0; _client_pointer < MAX_CONNECTIONS; _client_pointer++)
    {           
      if(client_list[_client_pointer]._pid!=-1)
	kill(client_list[_client_pointer]._pid,SIGCONT);
    } 
}





void *server_init(void *arg)
{
  int _server_sock_fd, _client_sock_fd;
  int _client_pointer;
  int _server_len, _client_len;
  char _server_full_mesg[]=SERVER_FULL_MESSAGE;
  unsigned char _server_full_flag=0;
  pthread_t _a_thread;
  
  /* set up socket */
  struct sockaddr_in _server_add;
  struct sockaddr_in _client_add;
  _server_sock_fd=socket(AF_INET, SOCK_STREAM, 0);
  _server_add.sin_family=AF_INET;
  _server_add.sin_addr.s_addr=htonl(INADDR_ANY);
  _server_add.sin_port=htons(Command_line_options.port_num);
  _server_len=sizeof(_server_add);
  
  bind(_server_sock_fd,(struct sockaddr *)&_server_add,_server_len);
  listen(_server_sock_fd, 5);
  
  /* init client list to -1 */
  for(_client_pointer=0;_client_pointer<MAX_CONNECTIONS;_client_pointer++)
    {client_list[_client_pointer]._pid=-1;} 
  _client_pointer=0;
  
  while(1) {
    /* printf("server waiting\n"); */ /* Blocking wait for clients */
     _client_sock_fd=accept(_server_sock_fd,(struct sockaddr *)&_client_add,&_client_len);
     _server_full_flag=1;

     /* find open spot on client list */
     for(_client_pointer=0;_client_pointer<MAX_CONNECTIONS;_client_pointer++)
       {
	 if(client_list[_client_pointer]._pid==-1) /* if spot open, */
	   {                          /* copy socket to client struct */
	     client_list[_client_pointer]._socket=_client_sock_fd; 
	     client_list[_client_pointer]._pid=pthread_create(&_a_thread,NULL,client_handler,(void*)_client_pointer);
	     /* copy pid to client struct and clear flag */
	     _server_full_flag=0;
	     break;
	   }  /* go back and wait for another connection */
       }
     
     /* if server is full give message and disconnect from client socket */
     if(_server_full_flag)
       {   
	 send(_client_sock_fd,_server_full_mesg,strlen(_server_full_mesg),0);
	 close(_client_sock_fd);
       }
  }
}

