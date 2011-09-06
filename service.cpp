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

/**
 * TODO
 */
net::_service::_service() :
ports_master(), ports_max(0), ports(), read_s(1024) {
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

bool net::_service::add_port(port_t port, bool tcp) {
  std::ostringstream ostr;
  struct addrinfo hints;
  struct addrinfo* result, * curr;
  int fd;

  ostr << port;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family   = AF_UNSPEC;
  hints.ai_socktype = tcp ? SOCK_STREAM : SOCK_DGRAM;
  hints.ai_family   = AI_PASSIVE;

  if(getaddrinfo(NULL, ostr.str().c_str(), &hints, &result) != 0)
    return false;

  for(curr = result; curr != NULL; curr = curr->ai_next) {
    fd = create_soc(curr->ai_family, curr->ai_socktype, curr->ai_protocol);

    if(fd == -1)
      continue;

    if(bind(fd, curr->ai_addr, curr->ai_addrlen) == 0)
      break;

    soc_close(fd);
  }

  if(curr == NULL)
    return false;

  freeaddrinfo(result);
  result = NULL;
  curr   = NULL;

  FD_SET(fd, &ports_master);
  ports[port] = fd;
  ports_max = std::max(fd, ports_max);

  soc_listen(fd, PENDING);

  return true;
}

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
    fd = create_soc(curr->ai_family, curr->ai_socktype, curr->ai_protocol);

    if(fd == -1)
      continue;

    if(bind(fd, curr->ai_addr, curr->ai_addrlen) == 0)
      break;

    soc_close(fd);
  }

  if(curr == NULL)
    return 0;

  port = ((struct sockaddr_in*)curr->ai_addr)->sin_port;
  FD_SET(fd, &ports_master);
  ports[fd] = fd;
  ports_max = std::max(fd, ports_max);

  soc_listen(fd, PENDING);

  return port;
}

net::sync_service::sync_service() :
master(), master_max(0), socs() {
  FD_ZERO(&master);
}

void net::sync_service::close() {
  closing = true;
}

