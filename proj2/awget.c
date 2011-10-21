/*
 * awget.h
 *
 *  Created on: Oct 4, 2011
 *      Author: Alex Norton,
 *              Nick Savage,
 *              Joshua Sorensen,
 *              Jef Mallal
 */

#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
/* networking includes */
#include <netdb.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "awget.h"

#define BUFFSIZE 8192


int socketSetup(char* host, char* port) {
  //setup the socket
  int s;
  struct addrinfo  hints;
  struct addrinfo* servs, * next = NULL;

  /* get server address information */
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family   = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  if(getaddrinfo(host, port, &hints, &servs) == -1) {
    perror("ERROR: main: getaddrinfo() failed for first SS");
    return -1;
  }

  /* create the socket connection */
  for(next = servs; next != NULL; next = next->ai_next) {
    if((s = socket(next->ai_family, hints.ai_socktype, next->ai_protocol)) < 0) {
      perror("ERROR: main: call to socket failed");
      continue;
    }

    if(connect(s, next->ai_addr, next->ai_addrlen) == -1) {
      perror("ERROR: main: call to connect failed");
      continue;
    }

    break;
  }

  if(next == NULL) {
    perror("ERROR: main: unable to connect to first ss");
    return -1;
  }

  freeaddrinfo(servs);
  return s;
}



//$awget <URL> [-c chainfile]
int main(int argc, char** argv) {
  FILE *fp;
  char* filename = "chaingang.txt";
  char* url = NULL;
  char* curr = NULL;
  int c;
  char buff[BUFFSIZE];
  list_header header;
  list_element elements[BUFFSIZE];


  srand(time(NULL));

  while((c= getopt(argc, argv, "c:")) > 0) {
    switch(c) {
      case 'c':
        filename = optarg;
        break;
    }
  }
  url = argv[optind];
  fp = fopen(filename,"r");
  if(!fp) {
    perror("ERROR: main: error opening stepping stone file.");
    return -1;
  }
  
  if (fgets(buff, BUFFSIZE, fp) == NULL) {
    perror("ERROR: main: File is empty.");
    return -1;
  }

  header.version = 1;
  header.l_size = atoi(buff);
  memset(header.f_name, '\0', sizeof(header.f_name));
  strncpy(header.f_name, url, sizeof(header.f_name));
                                
  memset(elements, 0, sizeof(elements));

  //pack element list
  for(c = 0; c < header.l_size; c++) {
    if(fgets(buff, BUFFSIZE, fp) == NULL) {
      perror("ERROR: main: Insufficient stepping stone size given.");
      return -1;
    }
    curr = strrchr(buff, ' ') + 1;
    *strchr(buff, ',') = '\0';

    strncpy(elements[c].host, buff, sizeof(elements[c].host));
    strncpy(elements[c].port, curr, sizeof(elements[c].port));
  }

  int hsize = header.l_size;
  //pick random element to send to and decrement the size of the list
  int r = rand() % header.l_size--;

  //setup socket
  int s = socketSetup(elements[r].host, elements[r].port);

  //send the header
  if(send(s, &header, sizeof(list_header), 0) == -1) {
    perror("ERROR: main: failed to send list header");
   return -1;
  }
  //send all other stepping stones except the one being sent to
  for(c = 0; c < hsize; c++) {
    if(r != c) {
      if(send(s, &elements[c], sizeof(list_element), 0) == -1) {
        perror("ERROR: main: failed to send element");
        return -1;
      }
    }
  }

  //receive file back and output

  file_header recInfo;

  if(recv(s, &recInfo, sizeof(file_header), 0) == -1) {
    perror("ERROR: main: failed to recieve file_header");
      return -1;
  }
  
  FILE * recFile;
  if(!(recFile = fopen("foo","w"))) {
    perror("ERROR: main: error opening recieve file.");
    return -1;
  }
  
  int recieved = 0;
  while(recieved != recInfo.f_size) {
      int inBytes = 0;
      memset(&buff, 0, sizeof(buff));
      if((inBytes = recv(s, &buff, sizeof(BUFFSIZE), 0)) == -1) {
        perror("ERROR: main: failed to recieve file");
        return -1;
      } else if (inBytes == 0){
        perror("ERROR: main: connection closed unexpectedly");
        return -1;
      }
      //print to the recFile
      fprintf(recFile, "%s", buff);
  }


  close(s); 
  fclose(fp);
  return 0;
}


