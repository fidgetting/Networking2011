/*
 * socket.cpp
 *
 *  Created on: Aug 25, 2011
 *      Author: norton
 */

/* stdlibrary includes */
#include <cstdio>
#include <cstdlib>
#include <cstring>

/* include class declaration */
#include <socket.hpp>

/**
 * Basic Constructor, sets everything to a default value.
 */
net::socket::socket() :
    _fd(0), _refs(NULL), _conn(false), _tcp(false), _src(NULL), _len(0) { }

/**
 * Creates a socket object using a file descriptor.
 *
 * @param fd the file descriptor
 */
net::socket::socket(const int& fd, sockaddr* src, socklen_t len) :
    _fd(fd), _refs(new int(1)), _conn(true), _tcp(false),
    _src((sockaddr*)calloc(1, len)), _len(len) {
  if(_refs == NULL || _src == NULL) {
    delete _refs;
    delete _src;
    _fd   = -1;
    _refs = NULL;
    _conn = false;
    _tcp  = false;
    _src  = NULL;
    _len  = 0;

    perror("net::socket::socket :: out of memory");
    return;
  }

  memset(_src,   0, _len);
  memcpy(_src, src, _len);
}

/**
 * Constructor that will attempt to connect to a host on a certain port. if this
 * is unable to connect, it will
 *
 * @param str  string name of the host
 * @param port string port to connect to on the host
 * @param tcp  if the connection will be tcp or udp
 */
net::socket::socket(const std::string& host, const std::string& port, bool tcp):
    _fd(0), _refs(NULL), _conn(false), _tcp(tcp), _src(NULL), _len(0) {
  connect(host, port, tcp);
}

/**
 * Copy constructor for the socket class. This simply copies the fields and
 * increments the reference counter.
 *
 * @param cpy the socket to copy
 */
net::socket::socket(const socket& cpy) :
    _fd(cpy._fd), _refs(cpy._refs), _conn(cpy._conn), _tcp(cpy._tcp),
    _src(cpy._src), _len(cpy._len) {
  if(_conn) {
    (*_refs)++;
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

  _fd   = asn._fd;
  _refs = asn._refs;
  _conn = asn._conn;
  _tcp  = asn._tcp;
  _src  = asn._src;
  _len  = asn._len;

  if(_conn) {
    (*_refs)++;
  }

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

  std::memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family   = AF_UNSPEC;
  hints.ai_socktype = tcp ? SOCK_STREAM : SOCK_DGRAM;

  if(getaddrinfo(host.c_str(), port.c_str(), &hints, &servers) != 0)
    { /* TODO throw exception */ }

  for(curr = servers; curr != NULL; curr = curr->ai_next) {
    _fd = soc_socket(
        curr->ai_family,
        hints.ai_socktype | SOCK_CLOEXEC,
        curr->ai_protocol);

    if(_fd < 0) {
      perror("net::socket::connect :: socket() call failed");
      continue;
    }

    if(tcp && soc_connect(_fd, curr->ai_addr, curr->ai_addrlen ) == -1) {
      soc_close(_fd);
      perror("net::socket::connect :: connect() call failed");
      continue;
    }
  }

  if(curr == NULL)
    { /* TODO throw exception */ }

  if(!tcp) {
    _src = (sockaddr*)calloc(1, curr->ai_addrlen);
    _len = curr->ai_addrlen;
    memset(_src,  0, _len);
    memcpy(_src, curr->ai_addr, _len);
  }

  _refs = new int(1);
  _conn = true;
  _tcp  = tcp;

  freeaddrinfo(servers);
}

/**
 * function to check if the file descriptor that is the actual socket should
 * be closed. This is called by the destructor, and assignment operators since
 * they imply a closing socket.
 */
void net::socket::check_close() {
  if(_conn) {
    /* decriment the reference counter to the socket */
    (*_refs)--;

    /* if this is the last socket object to the fd, close */
    /* the file descriptor that is the actual socket      */
    if(*_refs == 0) {
      delete _refs;
      delete _src;

      soc_close(_fd);

      _fd   = -1;
      _refs = NULL;
      _conn = false;
      _tcp  = false;
      _src  = NULL;
      _len  = 0;
    }
  }
}

