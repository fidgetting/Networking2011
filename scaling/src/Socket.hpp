/*
 * Socket.hpp
 *
 *  Created on: Apr 2, 2012
 *      Author: norton
 */

#pragma once

/* std includes */
#include <exception>
#include <functional>
#include <map>
#include <memory>
#include <stdint.h>
#include <string>
#include <vector>

/* TODO remove */
#include <iostream>

namespace soc {

  class Socket;
  class Stream;
  class Message;

  class Socket {
    public:

      enum type {
        none,
        server,
        client
      };

      Socket();
      Socket(uint16_t port, uint32_t backlog = 10);
      Socket(const std::string& host, uint16_t port);
      Socket(int32_t soc, type _type);
      Socket(const Socket& cpy);
      Socket(Socket&& that);

      virtual ~Socket();

      const Socket& operator=(const Socket& soc);

      inline bool operator==(const Socket& oth) const { return _fd == oth._fd; }
      inline bool operator< (const Socket& oth) const { return _fd <  oth._fd; }

      inline std::string host() const { return _host; }
      inline uint16_t    port() const { return _port; }
      inline uint32_t    fd()   const { return _fd;   }

      inline bool is_connected()  const { return _type == client && _fd > 0; }
      inline bool is_acceptable() const { return _type == server && _fd > 0; }

      Socket Accept() const;

      ssize_t Send(const void* data, size_t size, int flags) const;
      ssize_t Recv(      void* data, size_t size, int flags) const;

      template<typename ret_t, typename pass_t>
      inline ret_t exec(pass_t&& passed) const { return passed(_fd); }

    protected:

      /* information about how the socket is connected */
      std::string _host;  ///< The host that the socket is connected to
      uint16_t    _port;  ///< THe port that is connected to or listening on

      /* relating to the actual file descriptor */
      type      _type;    ///< is this a or client socket
      int32_t   _fd;      ///< the file descriptor that is the actual socket
      uint32_t* _count;   ///< reference count for the file descriptor
  };

  class Stream {
    public:

      Stream() : _source() { }
      Stream(Socket _soruce) : _source(_soruce) { }
      virtual ~Stream() { }

      inline void Write(const void* buffer, int offset, int len) const
        { _source.Send(((char*)buffer) + offset, len, 0); }
      void Read(void* buffer, int offset, int len) const
        { _source.Recv(((char*)buffer) + offset, len, 0); }

      const Stream& operator<<(bool val) const;
      const Stream& operator<<(short val) const;
      const Stream& operator<<(unsigned short val) const;
      const Stream& operator<<(int val) const;
      const Stream& operator<<(unsigned int val) const;
      const Stream& operator<<(long val) const;
      const Stream& operator<<(unsigned long val) const;
      const Stream& operator<<(float val) const;
      const Stream& operator<<(double val) const;
      const Stream& operator<<(long double val) const;
      const Stream& operator<<(std::string val) const;

      const Stream& operator>>(bool& val) const;
      const Stream& operator>>(short& val) const;
      const Stream& operator>>(unsigned short& val) const;
      const Stream& operator>>(int& val) const;
      const Stream& operator>>(unsigned int& val) const;
      const Stream& operator>>(long& val) const;
      const Stream& operator>>(unsigned long& val) const;
      const Stream& operator>>(float& val) const;
      const Stream& operator>>(double& val) const;
      const Stream& operator>>(long double& val) const;
      const Stream& operator>>(std::string& val) const;

      const Stream& operator<<(const Message& msg) const;
      const Stream& operator>>(Message& msg) const;

      /* stl containers */
      template<typename _T1, typename _T2>
      const Stream& operator<<(const std::pair<_T1, _T2> _p) const;
      template<typename _T1, typename _T2>
      const Stream& operator>>(std::pair<_T1, _T2>& _p) const;
      template<typename _Tp>
      const Stream& operator<<(const std::vector<_Tp>& _v) const;
      template<typename _Tp>
      const Stream& operator>>(std::vector<_Tp>& _v) const;
      template<typename _T1, typename _T2>
      const Stream& operator<<(const std::map<_T1, _T2>& _m) const;
      template<typename _T1, typename _T2>
      const Stream& operator>>(std::map<_T1, _T2>& _m) const;

    protected:

      Socket _source;
  };

  class Message {
    public:

      Message() { }
      virtual ~Message() { }

      virtual void Serialize  (const Stream& dst) const = 0;
      virtual void Deserialize(const Stream& src) = 0;

      template<typename _Callable, typename... _Args>
      static void Register(_Callable&& _func, _Args&&... _args);

      static std::shared_ptr<soc::Message> Get(const Stream& src);

      static uint16_t                                          _type_gen;
      static std::map<void*, uint16_t>                         _type_map;
      static std::map<uint16_t, std::function<Message*(void)>> _fact_map;
  };

  /**
   * TODO
   *
   * @param _p
   * @return
   */
  template<typename _T1, typename _T2>
  const Stream& Stream::operator<<(const std::pair<_T1, _T2> _p) const {
    this->operator<<(_p.first);
    this->operator<<(_p.second);
    return *this;
  }

  /**
   * TODO
   *
   * @param _p
   * @return
   */
  template<typename _T1, typename _T2>
  const Stream& Stream::operator>>(std::pair<_T1, _T2>& _p) const {
    this->operator>>(_p.first);
    this->operator>>(_p.second);
    return *this;
  }

  /**
   * TODO
   *
   * @param _v
   * @return
   */
  template<typename _Tp>
  const Stream& Stream::operator<<(const std::vector<_Tp>& _v) const {
    uint32_t size = _v.size();
    this->operator<<(size);
    for(auto curr : _v) {
      this->operator<<(curr);
    }
  }

  /**
   * TODO
   *
   * @param _v
   * @return
   */
  template<typename _Tp>
  const Stream& Stream::operator>>(std::vector<_Tp>& _v) const {
    uint32_t size;
    _Tp curr;
    _v.clear();

    this->operator>>(size);
    for(int i = 0; i < size; i++) {
      this->operator>>(curr);
      _v.push_back(curr);
    }
  }

  /**
   * TODO
   *
   * @param _m
   * @return
   */
  template<typename _T1, typename _T2>
  const Stream& Stream::operator<<(const std::map<_T1, _T2>& _m) const {
    uint32_t size = _m.size();
    this->operator<<(size);
    for(auto curr : _m) {
      this->operator<<(curr);
    }
  }

  /**
   * TODO
   *
   * @param _m
   * @return
   */
  template<typename _T1, typename _T2>
  const Stream& Stream::operator>>(std::map<_T1, _T2>& _m) const {
    uint32_t size;
    _T1 curr_key;
    _T2 curr_val;
    _m.clear();

    this->operator>>(size);
    for(int i = 0; i < size; i++) {
      this->operator>>(curr_key);
      this->operator>>(curr_val);

      _m[curr_key] = curr_val;
    }
  }

  /**
   * TODO
   *
   * @param _func
   * @param args
   */
  template<typename _Callable, typename... _Args>
  void
  Message::Register(_Callable&& _func, _Args&&... _args) {
    std::function<Message*(void)> fact =
        std::bind<Message*>(
            std::forward<_Callable>(_func),
            std::forward<_Args>(_args)...);

    Message* msg  = fact();
    uint16_t type = _type_gen++;
    void*    id   = *((void**)msg);

    _type_map[id]   = type;
    _fact_map[type] = fact;

    delete msg;
  }
}

namespace std {

  /**
   * TODO
   */
  template<>
  class hash<soc::Socket> {
    public:
      size_t operator()(const soc::Socket& soc) {
        return std::hash<int>()(soc.fd());
      }
  };
}
