/*
 * awget.h
 *
 *  Created on: Oct 4, 2011
 *      Author: Alex Norton,
 *              Nick Savage,
 *              Joshua Sorensen,
 *              Jef Mallal
 */

/* local includes */
#include <awget.h>

/* c std library includes */
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* networking includes */
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUF_SIZE 8192

#define dump_one(fd, format, ...)       \
  sprintf(buffer, format, __VA_ARGS__); \
  perror(buffer);                       \
  close(fd);                            \
  pthread_exit(-1)

#define dump_two(fd1, fd2, format, ...) \
  sprintf(buffer, format, __VA_ARGS__); \
  perror(buffer);                       \
  close(fd1);                           \
  close(fd2);                           \
  pthread_exit(-1)

/* ************************************************************************** */
/* *** connection function ************************************************** */
/* ************************************************************************** */

/**
 * Creates the connection to the next stepping stone
 *
 * @param host the address of the next stepping stone
 * @param port the port to connect on for the next stepping stone
 * @return the socket on success, -1 on error
 */
int next_ss(char* host, char* port) {
  int fd;
  struct addrinfo  hints;
  struct addrinfo* servs, * curr = NULL;

  /* get server address information */
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family   = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  if(getaddrinfo(host, port, &hints, &servs) == -1) {
    perror("ERROR: next_ss: getaddrinfo() failed for next SS");
    return -1;
  }

  /* create the socket connection */
  for(curr = servs; curr != NULL; curr = curr->ai_next) {
    if((fd = socket(curr->ai_family, hints.ai_socktype, curr->ai_protocol)) < 0) {
      perror("ERROR: next_ss: call to socket failed");
      continue;
    }

    if(connect(fd, curr->ai_addr, curr->ai_addrlen) == -1) {
      perror("ERROR: next_ss: call to connect failed");
      continue;
    }

    break;
  }

  if(curr == NULL) {
    perror("ERROR: next_ss: unable to connect to next ss") {
      return -1;
    }
  }

  freeaddrinfo(servs);
  return fd;
}

/**
 * TODO
 *
 * @param s
 */
void last_ss(int s) {
  file_header file_f;
  char*       file;
  char buffer[BUF_SIZE];

  /* TODO */
}

/**
 * TODO
 *
 * @param s
 * @param list_h
 */
void curr_ss(int s, list_header list_h) {
  ssize_t bytes;
  list_element list[1024];
  file_header  file;
  char buffer[BUF_SIZE];
  int i, fd, next;

  /* start printout */
  memset(buffer, '\0', sizeof(buffer));
  sprintf(buffer, " Request: %s\n chainlist is\n", list_h.f_name);

  /* receive the chain list */
  for(i = 0; i < list_h.l_size; i++) {
    if(recv(s, (void*)&list[i], sizeof(list_element), 0) == -1) {
      dump_one(s, "ERROR: connection: failed to receive list element %d", i);
    }
    sprintf(buffer + strlen(buffer),
        " <%s, %s>\n", list[i].host, list[i].port);
  }

  /* pick random next stepping stone */
  next = ((unsigned int)rand()) % list_h.l_size;
  sprintf(buffer + strlen(buffer), " next SS is <%s, %s>\n", list[next].host, list[next].port);
  if((fd = next_ss(list[next].host, list[next].port)) == -1) {
    close(s);
    pthread_exit(-1);
  }

  /* print the logging messages */
  sprintf(buffer + strlen(buffer),
      " waiting for file...\n...\n");
  printf("%s", buffer);

  /* send list to next stepping stone */
  list_h.l_size--;
  if(send(fd, (void*)&list_h, sizeof(list_header), 0) == -1) {
    dump_two(s, fd, "ERROR: connection: unable to send list header to next ss");
  }

  for(i = 0; i < list_h.l_size + 1; i++) {
    if(i != next) {
      if(send(fd, (void*)&list[i], sizeof(list_element), 0) == -1) {
        dump_two(s, fd, "ERROR: connection: unable to send list element %d", i);
      }
    }
  }

  /* wait for the file */
  if(recv(fd, (void*)&file, sizeof(file_header), 0) == -1) {
    dump_two(s, fd, "ERROR: connection: unable to receive file information");
  }

  for(i = 0; i < file.f_size; i += bytes) {
    memset(buffer, '\0', sizeof(buffer));

    if((bytes = recv(fd, (void*)buffer, sizeof(buffer), 0)) == -1) {
      dump_two(s, fd, "ERROR: connection: unable to receive bytes %d -> %d", i, i + sizeof(buffer));
    }

    if(send(s, buffer, bytes, 0) == -1) {
      dump_two(s, fd, "ERROR: connection: unable to send %d bytes to previous ss", bytes);
    }
  }

  printf(" Relaying file...\n Goodbuy\n");
  close(fd);
}

/**
 * This handles an incoming connection. Since the stepping stone doesn't create
 * a request it only handles a request, this is where all the work is really
 * done.
 *
 * It is important to note that the arguments for this function are weird. All
 * that needs to be passed to this function is the number of the socket that it
 * is talking on. Since the socket is a file descriptor, this only needs to be a
 * number between 0 and 1024, meaning that the number can easily fit in the size
 * of a pointer. So the given arguments are not a pointer, but actually an int.
 * This is done this way to avoid using the heap.
 *
 * @param args the socket that this connection should handle.
 */
void* connection(void* args) {
  int s = (int)args;
  list_header list_h;

  if(recv(s, (void*)&list_h, sizeof(list_header), 0) == -1) {
    dump_one(s, "ERROR: connection: failed to receive initial header");
  }

  if(list_h.l_size == 0) {
    last_ss(s);
  } else {
    curr_ss(s, list_h);
  }

  close(s);
  pthread_exit(0);
  return NULL;
}

/* ************************************************************************** */
/* *** main function ******************************************************** */
/* ************************************************************************** */

int main(int argc, char** argv) {

  /* general setup */
  srand(time(NULL));




}

