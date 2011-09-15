/*
 * server.cpp
 *
 *  Created on: Aug 29, 2011
 *      Author: norton
 */

#define PENDING 16

#include <service.hpp>

#include <sstream>
#include <string>

#define BACKLOG 16

/**
 * TODO
 */
net::_service::_service() :
    ports_master(), ports_max(0), ports(), udp_socs(), read_s(1024) {
  FD_ZERO(&ports_master);
}

/**
 * TODO
 */
net::_service::~_service() {
  FD_ZERO(&ports_master);

  for(iterator iter = ports.begin(); iter != ports.end(); iter++) {
    soc_close(iter->second);
  }
}

/**
 * TODO
 *
 * @param port
 * @param tcp
 * @return
 */
void net::_service::add_port(port_t port, bool tcp) {
  std::ostringstream ostr;
  struct addrinfo hints;
  struct addrinfo* result, * curr;
  int fd;
  int yes = 1;

  ostr << port;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family   = AF_UNSPEC;
  hints.ai_socktype = tcp ? SOCK_STREAM : SOCK_DGRAM;
  hints.ai_flags    = AI_PASSIVE;

  if((fd = getaddrinfo(NULL, ostr.str().c_str(), &hints, &result)) != 0) {
    fprintf(stderr, "net::_service::add_port :: getaddrinfo() call failed: %s\n",
        gai_strerror(fd));
    throw std::exception();
  }

  for(curr = result; curr != NULL; curr = curr->ai_next) {
    fd = soc_socket(curr->ai_family, curr->ai_socktype, curr->ai_protocol);

    if(fd == -1) {
      perror("net::_service::add_port :: socket() call failed");
      continue;
    }

    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      perror("net::_service::add_port :: setsockopt() call failed");
      throw std::exception();
    }

    if(bind(fd, curr->ai_addr, curr->ai_addrlen) == 0)
      break;

    perror("net::_service::add_port :: bind() call failed");
    soc_close(fd);
  }

  if(curr == NULL) {
    throw std::exception();
  }

  freeaddrinfo(result);
  result = NULL;
  curr   = NULL;

  if(tcp && listen(fd, BACKLOG)) {
    perror("net::_service::add_port :: listen() call failed");
  }

  FD_SET(fd, &ports_master);
  ports[port] = fd;
  ports_max = std::max(fd, ports_max);
  fd_blar = fd;
  this->tcp = tcp;

  soc_listen(fd, PENDING);
}

/**
 * TODO
 *
 * @param tcp
 * @return
 */
net::port_t net::_service::add_any_port(bool tcp) {
  struct addrinfo hints;
  struct addrinfo* result, * curr;
  int fd;
  port_t port;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family   = AF_UNSPEC;
  hints.ai_socktype = tcp ? SOCK_STREAM : SOCK_DGRAM;
  hints.ai_family   = AI_PASSIVE;

  if(getaddrinfo(NULL, NULL, &hints, &result) != 0)
    return 0;

  for(curr = result; curr != NULL; curr = curr->ai_next) {
    fd = soc_socket(curr->ai_family, curr->ai_socktype, curr->ai_protocol);

    if(fd == -1) {
      // TODO
      continue;
    }

    if(tcp && bind(fd, curr->ai_addr, curr->ai_addrlen) == 0)
      break;

    soc_close(fd);
  }

  if(curr == NULL)
    return 0;

  port = ((struct sockaddr_in*)curr->ai_addr)->sin_port;
  FD_SET(fd, &ports_master);
  ports[port] = fd;
  ports_max = std::max(fd, ports_max);

  if(!tcp) {
    udp_socs[fd] = net::sync_socket(fd, curr->ai_addr, curr->ai_addrlen);
  }

  soc_listen(fd, PENDING);

  return port;
}

/**
 * TODO
 */
net::sync_service::sync_service() :
master(), master_max(0), tcp_socs(), closing(false) {
  FD_ZERO(&master);
}

/**
 * TODO
 */
void net::sync_service::close() {
  closing = true;
}

