/*
 * socket.hpp
 *
 *  Created on: Aug 25, 2011
 *      Author: norton
 */

#ifndef SOCKET_HPP_INCLUDE
#define SOCKET_HPP_INCLUDE

#include <iostream>
#include <string>
using std::string;

/* networking includes */
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

namespace net {

  extern int    (*soc_close)   (int);
  extern int    (*soc_listen)  (int, int);
  extern int    (*soc_socket)  (int, int, int);
  extern ssize_t(*soc_recv)    (int, void*, size_t, int);
  extern ssize_t(*soc_send)    (int, const void*, size_t, int);
  extern int    (*soc_accept)  (int, struct sockaddr*, socklen_t*);
  extern int    (*soc_connect) (int, const struct sockaddr*, socklen_t);
  extern ssize_t(*soc_recvfrom)(int, void*, size_t, int, sockaddr*, socklen_t*);
  extern ssize_t(*soc_sendto)  (int, const void*, size_t, int, const sockaddr*, socklen_t);

  class sync_socket {
    public:

      /* constructors */
      sync_socket();
      sync_socket(const int& fd);
      sync_socket(const int& fd, sockaddr* src, socklen_t len);
      sync_socket(const string& host, const string& port, bool tcp = true);
      sync_socket(const sync_socket& cpy);

      /* destructors */
      virtual ~sync_socket();

      /* assignment operators */
      const sync_socket& operator=(const sync_socket& asn);

      /* functionality */
      void connect(const string& host, const string& port, bool tcp = true);

      template<typename _t>
      int  send(const _t* buf, const size_t len, int flags = 0) const;

      template<typename _t>
      int  recv(      _t* buf, const size_t len, int flags = 0) const;

      /* getters/setters */
      inline bool connected() const { return _conn; }
      inline  operator bool() const { return _conn; }

      inline int  fd()  const { return _fd;   }
      inline bool tcp() const { return _tcp;  }
      inline bool udp() const { return !_tcp; }

    protected:

      void check_close();

      int       _fd;
      int*      _refs;
      bool      _conn;
      bool      _tcp;
      sockaddr* _src;
      socklen_t _len;
  };

  /**
   * TODO
   *
   * @param buf
   * @param len
   * @param flags
   * @return
   */
  template<typename _t>
  int net::sync_socket::send(const _t* buf, const size_t len, int flags) const {
    if(_tcp)
      return soc_send(_fd, buf, len, flags);
    return soc_sendto(_fd, buf, len, flags, _src, _len);
  }

  /**
   * TODO
   *
   * @param buf
   * @param len
   * @param flags
   * @return
   */
  template<typename _t>
  int net::sync_socket::recv(_t* buf, const size_t len, int flags) const {
    socklen_t cpy = _len;

    if(_tcp)
      return soc_recv  (_fd, buf, len, flags);
    return soc_recvfrom(_fd, buf, len, flags, _src, &cpy);
  }
}

#endif /* SOCKET_HPP_INCLUDE */
