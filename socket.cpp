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
#include <exception>

/* include class declaration */
#include <socket.hpp>

int    (*net::soc_close)   (int) = close;
int    (*net::soc_listen)  (int, int) = listen;
int    (*net::soc_socket)  (int, int, int) = socket;
ssize_t(*net::soc_recv)    (int, void*, size_t, int) = recv;
ssize_t(*net::soc_send)    (int, const void*, size_t, int) = send;
int    (*net::soc_accept)  (int, struct sockaddr*, socklen_t*) = accept;
int    (*net::soc_connect) (int, const struct sockaddr*, socklen_t) = connect;
ssize_t(*net::soc_recvfrom)(int, void*, size_t, int, sockaddr*, socklen_t*) = recvfrom;
ssize_t(*net::soc_sendto)  (int, const void*, size_t, int, const sockaddr*, socklen_t) = sendto;

/**
 * Basic Constructor, sets everything to a default value.
 */
net::sync_socket::sync_socket() :
    _fd(0), _refs(NULL), _conn(false), _tcp(false), _src(NULL), _len(0) { }

/**
 * Create a socket object using just a file descriptor. TCP
 *
 * TODO
 *
 * @param fd
 */
net::sync_socket::sync_socket(const int& fd) :
    _fd(fd), _refs(new int(1)), _conn(true), _tcp(true), _src(NULL), _len(0) {
  if(_refs == NULL) {
    _fd = -1;
    _refs = NULL;
    _conn = false;
    _tcp  = false;
    _src  = NULL;
    _len  = 0;

    perror("net::socket::socket :: out of memory");
    return;
  }
}

/**
 * Creates a socket object using a file descriptor. UDP
 *
 * TODO
 *
 * @param fd the file descriptor
 * @param src
 * @param len
 */
net::sync_socket::sync_socket(const int& fd, sockaddr* src, socklen_t len) :
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
net::sync_socket::sync_socket(const std::string& host, const std::string& port, bool tcp):
    _fd(0), _refs(NULL), _conn(false), _tcp(tcp), _src(NULL), _len(0) {
  connect(host, port, tcp);
}

/**
 * Copy constructor for the socket class. This simply copies the fields and
 * increments the reference counter.
 *
 * @param cpy the socket to copy
 */
net::sync_socket::sync_socket(const sync_socket& cpy) :
    _fd(cpy._fd), _refs(cpy._refs), _conn(cpy._conn), _tcp(cpy._tcp),
    _src(cpy._src), _len(cpy._len) {
  if(_conn) {
    (*_refs)++;
  }
}

/**
 * TODO
 */
net::sync_socket::~sync_socket() {
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
const net::sync_socket& net::sync_socket::operator=(const net::sync_socket& asn) {
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
void net::sync_socket::connect(const string& host, const string& port, bool tcp) {
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
    { throw std::exception(); }

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
void net::sync_socket::check_close() {
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

