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
#include <getopt.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* networking includes */
#include <netdb.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define BACKLOG  16
#define BUF_SIZE 8192

#define MIN(x, y) x < y ? x : y

#define dump_one(fd, ...)         \
		sprintf(buffer, __VA_ARGS__); \
		perror(buffer);               \
		close(fd);                    \
		pthread_exit(NULL)

#define dump_two(fd1, fd2, ...)   \
		sprintf(buffer, __VA_ARGS__); \
		perror(buffer);               \
		close(fd1);                   \
		close(fd2);                   \
		pthread_exit(NULL)

char m_host[1024];
char m_port[1024];

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
    perror("ERROR: next_ss: unable to connect to next ss");
    return -1;
  }

  freeaddrinfo(servs);
  return fd;
}

/**
 * This is called if there are not more stepping stones in the list. This will
 * call a wget, then mmap the file and send it across the network. Once sent
 * this will unmap the file and delete it from the file system.
 *
 * @param s the socket to send the file across
 * @param fname the name of the file to call wget on
 */
void last_ss(int s, char* fname) {
  int fd, bytes;
  file_header file_f;
  char*       f_data;
  char*       f_end;
  char*       cursor;
  struct stat sb;
  char buffer[256];

  snprintf(buffer, sizeof(buffer),
      "wget %s --output-document=file_%d.tmp --quiet", fname, s);

  system(buffer);

  snprintf(buffer, sizeof(buffer), "file_%d.tmp", s);
  if((fd = open(buffer, O_RDONLY)) == -1) {
    perror("ERROR: last_ss: couldn't open file after wget");
    return;
  }

  if(fstat(fd, &sb) == -1) {
    perror("ERROR: last_ss: couldn't fstat wget file");
    return;
  }

  if((f_data = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED) {
    perror("ERROR: last_ss: couldn't map file to memory");
    return;
  }

  file_f.version = 1;
  file_f.f_size = htonl((unsigned int)sb.st_size);
  if(send(s, &file_f, sizeof(file_header), 0) == -1) {
    perror("ERROR: last_ss: failed to send file header");
    return;
  }

  f_end = f_data + sb.st_size;
  for(cursor = f_data; cursor < f_end; cursor += bytes) {
    if((bytes = send(s, cursor, MIN(BUF_SIZE, f_end - cursor), 0)) == -1) {
      perror("ERROR: last_ss: failed to send file");
      return;
    }
  }

  if(close(fd) == -1)
    perror("ERROR: last_ss: couldn't close file");
  if(munmap(f_data, sb.st_size) == -1)
    perror("ERROR: last_ss: couldn't remove file from memory");
  if(remove(buffer) == -1)
    perror("ERROR: last_ss: couldn't remove file from filesystem");
}

/**
 * This is called if this stepping stone is not the last stepping stone in the
 * list.
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
  sprintf(buffer, "ss <%s, %s>:\n Request: %s\n chainlist is\n",
      m_host, m_port, list_h.f_name);

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
    pthread_exit(NULL);
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
      dump_two(s, fd, "ERROR: connection: unable to receive bytes %d -> %ld", i, i + sizeof(buffer));
    }

    if(send(s, buffer, bytes, 0) == -1) {
      dump_two(s, fd, "ERROR: connection: unable to send %ld bytes to previous ss", bytes);
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
  int s;
  char buffer[256];
  list_header list_h;

#if __x86_64__ | __amd64__
  s = (long)args;
#else
  s = (int)args;
#endif

  if(recv(s, (void*)&list_h, sizeof(list_header), 0) == -1) {
    dump_one(s, "ERROR: connection: failed to receive initial header");
  }

  list_h.l_size = ntohl(list_h.l_size);

  if(list_h.l_size == 0) {
    last_ss(s, list_h.f_name);
  } else {
    curr_ss(s, list_h);
  }

  close(s);
  pthread_exit(0);
  return NULL;
}

/**
 *
 *
 * @return
 */
int server(char* port) {
  int sockfd = -1;
  struct addrinfo hints;
  struct addrinfo* serv, * curr;
  struct sockaddr_storage their_addr;
  socklen_t their_size;
  pthread_t thread;
  int yes = 1;

#if __x86_64__ | __amd64__
  long fd;
#else
  int fd;
#endif

  serv = NULL;
  curr = NULL;

  /* initialize hints */
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family   = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags    = AI_PASSIVE;
  hints.ai_protocol = 0;
  hints.ai_canonname = NULL;
  hints.ai_addr = NULL;
  hints.ai_next = NULL;

  /* get possible ports to listen on */
  if((fd = getaddrinfo(NULL, port, &hints, &serv)) != 0) {
    fprintf(stderr, "ERROR: server: getaddrinfo call failed: %s\n",
        gai_strerror(fd));
    return -1;
  }

  for(curr = serv; curr != NULL; curr = curr->ai_next) {
    sockfd = socket(curr->ai_family, curr->ai_socktype, curr->ai_protocol);
    if(sockfd == -1) {
      perror("ERROR: server: socket call failed");
      continue;
    }

    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      perror("ERROR: server: setsockopt call failed");
      return -1;
    }

    if(bind(sockfd, curr->ai_addr, curr->ai_addrlen) == 0)
      break;

    perror("ERROR: server: bind call failed");
    close(sockfd);
  }

  /* make sure we have a socket */
  if(curr == NULL) {
    fprintf(stderr, "ERROR: server: unable to create listen socket\n");
    return -1;
  }
  freeaddrinfo(serv);

  /* perform book keeping */
  if((fd = getnameinfo(curr->ai_addr, curr->ai_addrlen,
      m_host, sizeof(m_host), m_port, sizeof(m_port), 0)) != 0) {
    fprintf(stderr, "ERROR: server: getnameinfo call failed: %s\n",
        gai_strerror(fd));
    return -1;
  }
  printf("ss <%s, %s>\n", m_host, m_port);

  if(listen(sockfd, BACKLOG) == -1) {
    perror("ERROR: server: listen call failed");
    return -1;
  }

  /* accept loop */
  for(;;) {
    their_size = sizeof(their_addr);
    fd = accept(sockfd, (struct sockaddr*)&their_addr, &their_size);
    if(fd == -1) {
      perror("ERROR: server: accept call failed");
      continue;
    }

    pthread_create(&thread, NULL, connection, (void*)fd);
    pthread_detach(thread);
  }

  return 0;
}

/* ************************************************************************** */
/* *** main function ******************************************************** */
/* ************************************************************************** */

int main(int argc, char** argv) {
  int c;
  char* port = "24516";

  /* general setup */
  srand(time(NULL));

  if((c = getopt(argc, argv, "p:"))) {
    switch(c) {
      case 'p': port = optarg; break;
      default: /* TODO usage */ break;
    }
  }

  return server(port);
}

