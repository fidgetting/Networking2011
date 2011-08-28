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

  int(*create_soc )(int, int, int)                          = socket;
  int(*connect_soc)(int, const struct sockaddr*, socklen_t) = connect;
  ssize_t(*write_soc)(int, const void*, size_t)             = write;
  ssize_t(*read_soc)(int, void*, size_t)                    = read;

  class socket {
    public:

      /* constructors */
      socket();
      socket(const int& fd);
      socket(const string& host, const string& port, bool tcp = true);
      socket(const socket& cpy);

      /* destructors */
      virtual ~socket();

      /* assignment operators */
      const socket& operator=(const socket& asn);
      const int&    operator=(const int&    asn);

      /* functionality */
      void connect(const string& host, const string& port, bool tcp = true);

      template<typename _t>
      void write(const _t& msg) const;

      template<typename _t>
      _t* read() const;

      /* getters/setters */
      inline bool connected() const { return conn; }
      inline  operator bool() const { return conn; }

      //static socket listen(const string& port, bool tcp = true);

    protected:

      void check_close();

      int  fd;
      int* refs;
      bool conn;
  };

  /**
   * TODO
   *
   * @param msg
   */
  template<typename _t>
  void socket::write(const _t& msg) const {
    if(write_soc(fd, &msg, sizeof(_t)) != sizeof(_t))
        { /* throw exception */ }
  }

  /**
   * TODO
   *
   * @return
   */
  template<typename _t>
  _t* socket::read() const {
    _t* buf = new _t();

    if(read_soc(fd, buf, sizeof(_t)) != sizeof(_t))
      { /* throw exception */ }

    return buf;
  }
}

#endif /* SOCKET_HPP_INCLUDE */
