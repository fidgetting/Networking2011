/*
 * server.hpp
 *
 *  Created on: Aug 29, 2011
 *      Author: norton
 */

#ifndef SERVER_HPP_INCLUDE
#define SERVER_HPP_INCLUDE

#include <socket.hpp>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <algorithm>
#include <cstring>
#include <map>
#include <vector>

namespace net {

  typedef unsigned short port_t;

  /**
   * TODO
   */
  class _service {
    public:

      typedef std::map<port_t, int>  port_map_t;
      typedef port_map_t::iterator                iterator;
      typedef port_map_t::const_iterator          const_iterator;
      typedef port_map_t::reverse_iterator        reverse_iterator;
      typedef port_map_t::const_reverse_iterator  const_reverse_iterator;

      _service();
      virtual ~_service();

      virtual bool   add_port(port_t port, bool tcp = true);
      virtual port_t add_any_port(bool tcp = true);

      inline iterator               begin()        { return ports.begin();  }
      inline iterator               end()          { return ports.end();    }
      inline const_iterator         begin() const  { return ports.begin();  }
      inline const_iterator         end()   const  { return ports.end();    }
      inline reverse_iterator       rbegin()       { return ports.rbegin(); }
      inline reverse_iterator       rend()         { return ports.rend();   }
      inline const_reverse_iterator rbegin() const { return ports.rbegin(); }
      inline const_reverse_iterator rend()   const { return ports.rend();   }

      inline unsigned int  size() const { return read_s; }
      inline unsigned int& size()       { return read_s; }

    protected:

      fd_set                ports_master;
      int                   ports_max;
      port_map_t            ports;
      std::map<int, sync_socket> udp_socs;
      unsigned int          read_s;
  };

  /**
   * TODO
   */
  class sync_service : public _service {
    public:

      sync_service();
      virtual ~sync_service() { }

      template<typename _t, typename... args_t>
      void listen(_t& callback, args_t... args);

      void close();

    private:

      fd_set                     master;
      int                        master_max;
      std::vector<sync_socket>   tcp_socs;
      bool                       closing;
  };

  /**
   * TODO
   *
   * @param callback
   * @param args
   */
  template<typename _t, typename... args_t>
  void sync_service::listen(_t& callback, args_t... args) {
    fd_set use;
    sockaddr_storage addr;
    socklen_t addr_size;
    int fd;

    std::cout << "listening" << std::endl;
    memcpy(&master, &ports_master, sizeof(fd_set));
    master_max = ports_max;
    addr_size = sizeof(addr);

    while(!closing) {
      FD_ZERO(&use);

      memcpy(&use, &master, sizeof(fd_set));

      if(select(master_max, &use, NULL, NULL, NULL) < 0)
      { /* throw exception */ }

      for(iterator iter = begin(); iter != end(); iter++) {
        if(FD_ISSET(iter->second, &use)) {
          fd = iter->second;
          if(udp_socs.find(fd) == udp_socs.end()) {
            fd = soc_accept(fd, (sockaddr*)&addr, &addr_size);

            FD_SET(fd, &master);
            master_max = std::max(fd, master_max);
            tcp_socs.push_back(net::sync_socket(fd));
          } else if(!callback(udp_socs[fd], args...))
            FD_CLR(fd, &master);
        }
      }
    }

    for(auto iter = tcp_socs.begin(); iter != tcp_socs.end(); iter++) {
      if(FD_ISSET(iter->fd(), &use)) {
        if(!callback(*iter, args...)) {
          FD_CLR(iter->fd(), &master);
        }
      }
    }
  }
}

#endif /* SERVER_HPP_INCLUDE */
