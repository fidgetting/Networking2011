/*
 * socket.cpp
 *
 *  Created on: Aug 25, 2011
 *      Author: norton
 */

/* stdlibrary includes */
#include <cstdlib>
#include <cstring>

/* include class declaration */
#include <socket.hpp>

/**
 * Basic Constructor, sets everything to a default value.
 */
net::socket::socket() : fd(0), refs(NULL), conn(false) { }

/**
 * Creates a socket object using a file descriptor.
 *
 * @param fd the file descriptor
 */
net::socket::socket(const int& fd) : fd(fd), refs(new int(1)), conn(true) { }

/**
 * Constructor that will attempt to connect to a host on a certain port. if this
 * is unable to connect, it will
 *
 * @param str
 * @param port
 */
net::socket::socket(const std::string& host, const std::string& port, bool tcp)
: fd(0), refs(NULL), conn(false) {
  connect(host, port, tcp);
}

/**
 * TODO
 *
 * @param cpy
 */
net::socket::socket(const socket& cpy) : fd(0), refs(NULL), conn(false) {
  fd   = cpy.fd;
  refs = cpy.refs;
  conn = cpy.conn;

  if(conn) {
    (*refs)++;
  }
}

/**
 * TODO
 */
net::socket::~socket() {
  this->check_close();
}

/**
 * essentially creates a copy of a socket. This will check if the current
 * socket needs to be closed, and then assigns the fields of the other
 * socket.
 *
 * @param asn the socket to assign to
 * @return simply returns the parameter
 */
const net::socket& net::socket::operator=(const net::socket& asn) {
  this->check_close();

  fd   = asn.fd;
  refs = asn.refs;
  conn = asn.conn;

  if(conn) {
    (*refs)++;
  }

  return asn;
}

/**
 * assigns a socket to an already open socket descriptor.
 *
 * @param asn the descriptor to assign to
 * @return simply returns the parameter
 */
const int& net::socket::operator=(const int& asn) {
  this->check_close();

  fd  = asn;
  refs = new int(1);
  conn = true;

  return asn;
}

/**
 * TODO
 *
 * @param host
 * @param port
 * @param tcp
 */
void net::socket::connect(const string& host, const string& port, bool tcp) {
  this->check_close();

  struct addrinfo  hints;
  struct addrinfo* servers, * curr = NULL;
  char buf[8];

  std::memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family   = AF_UNSPEC;
  hints.ai_socktype = tcp ? SOCK_STREAM : SOCK_DGRAM;

  if(getaddrinfo(host.c_str(), port.c_str(), &hints, &servers) != 0)
    { /* TODO throw exception */ }

  for(curr = servers; curr != NULL; curr = curr->ai_next) {
    fd = create_soc(
        curr->ai_family,
        hints.ai_socktype | SOCK_CLOEXEC,
        curr->ai_protocol);

    if(fd < 0)
      continue;

    if(connect_soc(fd, curr->ai_addr, curr->ai_addrlen ) != -1)
      break;

    close(fd);
  }

  if(curr == NULL)
    { /* TODO throw exception */ }

  conn = true;
  refs = new int(1);

  freeaddrinfo(servers);
}

/*net::socket net::socket::listen(const string& port, bool tcp) {
  struct addrinfo  hints;
  struct addrinfo* result, * curr;
  int fd;

  std::memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family   = AF_UNSPEC;
  hints.ai_socktype = tcp ? SOCK_STREAM : SOCK_DGRAM;
  hints.ai_flags    = AI_PASSIVE;

  if(getaddrinfo(NULL, port.c_str(), &hints, &result) != 0)
    {  TODO throw exception  }

  for(curr = result; curr != NULL; curr = curr->ai_next) {




  }
}*/

/**
 * function to check if the file descriptor that is the actual socket should
 * be closed. This is called by the destructor, and assignment operators since
 * they imply a closing socket.
 */
void net::socket::check_close() {
  if(conn) {
    /* decriment the reference counter to the socket */
    (*refs)--;

    /* if this is the last socket object to the fd, close */
    /* the file descriptor that is the actual socket      */
    if(*refs == 0) {
      delete refs;

      close(fd);

      conn = false;
      fd   = 0;
      refs = NULL;
    }
  }
}

