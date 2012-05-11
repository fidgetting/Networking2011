/*
 * Socket.cpp
 *
 *  Created on: Apr 2, 2012
 *      Author: norton
 */

/* local includes */
#include <Socket.hpp>

/* std includes */
#include <cstring>
#include <sstream>

/* networking includes */
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>

/* ************************************************************************** */
/* ***  locals  ************************************************************* */
/* ************************************************************************** */

/**
 * TODO
 *
 * @param sa
 */
static void* get_in_addr(struct sockaddr* sa) {
  if(sa->sa_family == AF_INET)
    return &(((sockaddr_in*)sa)->sin_addr);
  return &(((sockaddr_in6*)sa)->sin6_addr);
}

/* ************************************************************************** */
/* ***  Stream methods  ***************************************************** */
/* ************************************************************************** */

/**
 * TODO
 *
 * @param str
 * @param val
 */
const soc::Stream& soc::Stream::operator<<(bool val) const {
  Write(&val, 0, sizeof(bool));
  return *this;
}

/**
 * TODO
 *
 * @param str
 * @param val
 */
const soc::Stream& soc::Stream::operator<<(short val) const {
  uint8_t data[2];

  data[0] = (val & 0xFF);
  data[1] = (val & (0xFF << 8)) >> 8;

  Write(data, 0, sizeof(short));
  return *this;
}

/**
 * TODO
 *
 * @param str
 * @param val
 */
const soc::Stream& soc::Stream::operator<<(unsigned short val) const {
  uint8_t data[2];

  data[0] = (val & 0xFF);
  data[1] = (val & (0xFF << 8)) >> 8;

  Write(data, 0, sizeof(unsigned short));
  return *this;
}

/**
 * TODO
 *
 * @param str
 * @param val
 */
const soc::Stream& soc::Stream::operator<<(int val) const {
  uint8_t data[4];

  data[0] = (val &  0xFF);
  data[1] = (val & (0xFF << 8 )) >> 8;
  data[2] = (val & (0xFF << 16)) >> 16;
  data[3] = (val & (0xFF << 24)) >> 24;

  Write(data, 0, sizeof(int));
  return *this;
}

/**
 * TODO
 *
 * @param str
 * @param val
 */
const soc::Stream& soc::Stream::operator<<(unsigned int val) const {
  uint8_t data[4];

  data[0] = (val &  0xFF);
  data[1] = (val & (0xFF << 8 )) >> 8;
  data[2] = (val & (0xFF << 16)) >> 16;
  data[3] = (val & (0xFF << 24)) >> 24;

  Write(data, 0, sizeof(unsigned int));
  return *this;
}

/**
 * TODO
 *
 * @param str
 * @param val
 */
const soc::Stream& soc::Stream::operator<<(long val) const {
  uint8_t data[8];

  data[0] = (val &  long(0xFF));
  data[1] = (val & (long(0xFF) << 8 )) >> 8;
  data[2] = (val & (long(0xFF) << 16)) >> 16;
  data[3] = (val & (long(0xFF) << 24)) >> 24;
  data[4] = (val & (long(0xFF) << 32)) >> 32;
  data[5] = (val & (long(0xFF) << 40)) >> 40;
  data[6] = (val & (long(0xFF) << 48)) >> 48;
  data[7] = (val & (long(0xFF) << 56)) >> 56;

  Write(data, 0, sizeof(long));
  return *this;
}

/**
 * TODO
 *
 * @param str
 * @param val
 */
const soc::Stream& soc::Stream::operator<<(unsigned long val) const {
  uint8_t data[8];

  data[0] = (val &  long(0xFF));
  data[1] = (val & (long(0xFF) << 8 )) >> 8;
  data[2] = (val & (long(0xFF) << 16)) >> 16;
  data[3] = (val & (long(0xFF) << 24)) >> 24;
  data[4] = (val & (long(0xFF) << 32)) >> 32;
  data[5] = (val & (long(0xFF) << 40)) >> 40;
  data[6] = (val & (long(0xFF) << 48)) >> 48;
  data[7] = (val & (long(0xFF) << 56)) >> 56;

  Write(data, 0, sizeof(unsigned long));
  return *this;
}

/**
 * TODO
 *
 * @param str
 * @param val
 */
const soc::Stream& soc::Stream::operator<<(float val) const {
  Write(&val, 0, sizeof(float));
  return *this;
}

/**
 * TODO
 *
 * @param str
 * @param val
 */
const soc::Stream& soc::Stream::operator<<(double val) const {
  Write(&val, 0, sizeof(double));
  return *this;
}

/**
 * TODO
 *
 * @param str
 * @param val
 */
const soc::Stream& soc::Stream::operator<<(long double val) const {
  Write(&val, 0, sizeof(long double));
  return *this;
}

/**
 * TODO
 *
 * @param str
 * @param val
 */
const soc::Stream& soc::Stream::operator<<(std::string val) const {
  uint32_t len = val.length();

  this->operator<<(len);
  Write((void*)val.c_str(), 0, len);
  return *this;
}

/**
 * TODO
 *
 * @param str
 * @param val
 */
const soc::Stream& soc::Stream::operator>>(bool& val) const {
  Read(&val, 0, sizeof(bool));
  return *this;
}

/**
 * TODO
 *
 * @param str
 * @param val
 */
const soc::Stream& soc::Stream::operator>>(short& val) const  {
  uint8_t data[2];

  Read(data, 0, sizeof(short));

  val = 0;
  val |= data[0] | (data[1] << 8);
  return *this;
}

/**
 * TODO
 *
 * @param str
 * @param val
 */
const soc::Stream& soc::Stream::operator>>(unsigned short& val) const  {
  uint8_t data[2];

  Read(data, 0, sizeof(unsigned short));

  val = 0;
  val |= data[0] | (data[1] << 8);
  return *this;
}

/**
 * TODO
 *
 * @param str
 * @param val
 */
const soc::Stream& soc::Stream::operator>>(int& val) const  {
  uint8_t data[4];

  Read(data, 0, sizeof(int));

  val = 0;
  val |= data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
  return *this;
}

/**
 * TODO
 *
 * @param str
 * @param val
 */
const soc::Stream& soc::Stream::operator>>(unsigned int& val) const  {
  uint8_t data[4];

  Read(data, 0, sizeof(unsigned int));

  val = 0;
  val |= data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
  return *this;
}

/**
 * TODO
 *
 * @param str
 * @param val
 */
const soc::Stream& soc::Stream::operator>>(long& val) const  {
  uint8_t data[8];

  Read(data, 0, sizeof(long));

  val = 0;
  val |= long(data[0]) | (long(data[1]) << 8) | (long(data[2]) << 16)
                    | (long(data[3]) << 24) | (long(data[4]) << 32)
                    | (long(data[5]) << 40) | (long(data[6]) << 48)
                    | (long(data[7]) << 56);
  return *this;
}

/**
 * TODO
 *
 * @param str
 * @param val
 */
const soc::Stream& soc::Stream::operator>>(unsigned long& val) const  {
  uint8_t data[8];

  Read(data, 0, sizeof(unsigned long));

  val = 0;
  val |= long(data[0]) | (long(data[1]) << 8) | (long(data[2]) << 16)
                    | (long(data[3]) << 24) | (long(data[4]) << 32)
                    | (long(data[5]) << 40) | (long(data[6]) << 48)
                    | (long(data[7]) << 56);
  return *this;
}

/**
 * TODO
 *
 * @param str
 * @param val
 */
const soc::Stream& soc::Stream::operator>>(float& val) const  {
  Read(&val, 0, sizeof(float));
  return *this;
}

/**
 * TODO
 *
 * @param str
 * @param val
 */
const soc::Stream& soc::Stream::operator>>(double& val) const  {
  Read(&val, 0, sizeof(double));
  return *this;
}

/**
 * TODO
 *
 * @param str
 * @param val
 */
const soc::Stream& soc::Stream::operator>>(long double& val) const  {
  Read(&val, 0, sizeof(long double));
  return *this;
}

/**
 * TODO
 *
 * @param str
 * @param val
 */
const soc::Stream& soc::Stream::operator>>(std::string& val) const  {
  uint32_t len;
  char* buf;

  this->operator>>(len);

  val.clear();
  buf = new char[len + 1];
  buf[len] = 0;
  Read(buf, 0, len);

  val.append(buf);
  delete[] buf;
  return *this;
}

/**
 * TODO
 *
 * @param dst
 */
const soc::Stream& soc::Stream::operator<<(const Message& msg) const {
  void* id = *((void**)&msg);
  uint16_t type = soc::Message::_type_map[id];

  this->operator<<(type);
  msg.Serialize(*this);

  return *this;
}

/**
 * TODO
 *
 * @param src
 */
const soc::Stream& soc::Stream::operator>>(Message& msg) const {
  msg.Deserialize(*this);
  return *this;
}

/* ************************************************************************** */
/* ***  Socket methods  ***************************************************** */
/* ************************************************************************** */

/**
 * @brief Create a Socket that cannot do anything
 */
soc::Socket::Socket() :
_host(""), _port(0), _type(none), _fd(-1), _count(NULL) { }

/**
 * @brief Creates a server socket listening on the specific port.
 *
 * @param port     the port to listen on
 * @param backlog  the backlog that is used for socket connections
 */
soc::Socket::Socket(uint16_t port, uint32_t backlog) :
_host(""),  _port(port), _type(server), _fd(-1), _count(NULL) {
  std::ostringstream ostr;
  addrinfo hint;
  addrinfo* serv;
  addrinfo* curr;
  int yes = 1;

  memset(&hint, 0, sizeof(struct addrinfo));
  hint.ai_family   = AF_UNSPEC;
  hint.ai_socktype = SOCK_STREAM;
  hint.ai_flags    = AI_PASSIVE;

  ostr << port;
  if((_fd = getaddrinfo(NULL, ostr.str().c_str(), &hint, &serv)) != 0) {
    // TODO exception
    return;;
  }

  for(curr = serv; curr != NULL; curr = curr->ai_next) {
    _fd = socket(curr->ai_family, curr->ai_socktype, curr->ai_protocol);
    if(_fd < 0) {
      continue;
    }

    if(setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      close(_fd);
      continue;
    }

    if(bind(_fd, curr->ai_addr, curr->ai_addrlen) == 0) {
      break;
    }

    close(_fd);
  }

  freeaddrinfo(serv);
  if(curr == NULL) {
    _fd = -1;
    return;
  }

  if(listen(_fd, backlog) == -1) {
    close(_fd);
    _fd = -1;

    // TODO exception
    return;
  }

  _count = new uint32_t(1);
}

/**
 * Sets up a client Socket. The creator of the socket should call is_connected
 * on this socket before using send or recv. An error could have occured in this
 * function that would prevent the socket from begin valid.
 *
 * @param host  the host to connect to
 * @param port  the port to connect to
 */
soc::Socket::Socket(const std::string& host, uint16_t port) :
_host(host), _port(port), _type(client), _fd(-1), _count(NULL){
  addrinfo hints;
  addrinfo* serv, * curr;
  std::ostringstream ostr_port;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family   = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  ostr_port << port;
  if(getaddrinfo(host.c_str(), ostr_port.str().c_str(), &hints, &serv) == -1) {
    return;
  }

  for(curr = serv; curr != NULL; curr = curr->ai_next) {
    _fd = socket(curr->ai_family, curr->ai_socktype, curr->ai_protocol);
    if(_fd < 0) {
      continue;
    }

    if(connect(_fd, curr->ai_addr, curr->ai_addrlen) == -1) {
      close(_fd);
      continue;
    }

    break;
  }

  if(curr == NULL) {
    close(_fd);
    return;
  }

  freeaddrinfo(serv);
  _count = new uint32_t(1);
}

/**
 * Used when a socket has already been created and the user wants it to be
 * correct managed by the soc::Socket interface.
 *
 * @param soc    the socket to manage
 * @param _type  the type of the socket, either client or server
 */
soc::Socket::Socket(int32_t soc, type _type) :
_host(""), _port(0),  _type(_type), _fd(soc), _count(new uint32_t(1)) { }

/**
 * Copies a Socket.
 *
 * @param cpy  the Socket to copy
 */
soc::Socket::Socket(const Socket& cpy) :
_host(cpy._host), _port(cpy._port), _type(cpy._type), _fd(cpy._fd),
_count(cpy._count) {
  (*_count)++;
}

soc::Socket::Socket(Socket&& that) {
  _host  = that._host;
  _port  = that._port;
  _type  = that._type;
  _fd    = that._fd;
  _count = that._count;
  (*_count)++;
}

/**
 * Decreases the reference count for the socket. If the reference count in then
 * 0, this will close the file descriptor;
 */
soc::Socket::~Socket() {
  if(_count != NULL) {
    (*_count)--;
    if(*_count == 0) {
      close(_fd);
      delete _count;

      _count = NULL;
      _fd = -1;
    }
  }
}

/**
 * Assignment operator for the Socket class.
 *
 * @param soc  the socket to assign to
 * @return     the socket that was passed as a parameter
 */
const soc::Socket& soc::Socket::operator=(const Socket& soc) {
  _host = soc._host;
  _port = soc._port;
  _type = soc._type;

  if(_count != NULL) {
    (*_count)--;
    if(*_count == 0) {
      delete _count;
      close(_fd);

      _count = NULL;
      _fd = -1;
    }
  }

  _count = soc._count;
  _fd    = soc._fd;

  (*_count)++;

  return soc;
}

/**
 * Accepts a new connection on a server socket. If this is called on a Socket
 * that is not is_acceptable(), this will return an invalid socket as well.
 *
 * @return  Socket connected to a client.
 */
soc::Socket soc::Socket::Accept() const {
  Socket ret;
  sockaddr_storage their_addr;
  socklen_t        their_size;
  char s[INET6_ADDRSTRLEN];
  int fd;

  if(!is_acceptable()) {
    return Socket();
  }

  their_size = sizeof(their_addr);
  fd = accept(_fd, (sockaddr*)&their_addr, &their_size);

  inet_ntop(their_addr.ss_family,
      get_in_addr((sockaddr*)&their_addr), s, sizeof(s));

  ret._host  = s;
  ret._port  = _port;
  ret._type  = client;
  ret._fd    = fd;
  ret._count = new uint32_t(1);

  return ret;
}

/**
 * Basic wrapper for the send system call.
 *
 * @param data   the data to send on the socket
 * @param size   the amount of data to send in bytes
 * @param flags  any special flags
 * @return -1 if it failed for some reason
 */
ssize_t soc::Socket::Send(const void* data, size_t size, int flags) const {
  int ret = 0;
  int total = 0;
  const char* ptr = (const char*)data;

  if(!is_connected());
  // TODO throw exception

  while(total != size) {
    if((ret = send(_fd, ptr + total, size - total, flags)) == -1)
      break;
    total += ret;
  }

  return ret;
}

/**
 * Basic wrapper for the recv system call.
 *
 * @param data   buffer to place the data in
 * @param size   the amount of data to receive in bytes
 * @param flags  any special flags
 * @return -1 if it failed for some reason
 */
ssize_t soc::Socket::Recv(void* data, size_t size, int flags) const {
  int ret = 0;
  int total = 0;
  char* ptr = (char*)data;

  if(!is_connected());
  // TODO throw exception

  while(total != size) {
    if((ret = recv(_fd, ptr + total, size - total, flags)) == -1)
      break;
    total += ret;
  }

  return ret;
}

/* ************************************************************************** */
/* ***  Message methods  **************************************************** */
/* ************************************************************************** */

uint16_t                                               soc::Message::_type_gen = 0;
std::map<void*, uint16_t>                              soc::Message::_type_map;
std::map<uint16_t, std::function<soc::Message*(void)>> soc::Message::_fact_map;
