/*
 * Server.hpp
 *
 *  Created on: Apr 2, 2012
 *      Author: norton
 */

#pragma once

/* local includes */
#include <Socket.hpp>

/* std includes */
#include <iostream>
#include <unordered_map>
#include <memory>

namespace soc {

  template<typename conn_t>
  class Server {
    public:

      Server(uint16_t port) : _port(port), _soc(port) { }

      template<typename Callback_t>
      void operator()(Callback_t&& callback) {
        fd_set socs;
        uint32_t max;

        while(true) {
          FD_ZERO(&socs);
          FD_SET(_soc.fd(), &socs);

          max = 0;
          max = std::max(_soc.fd(), max);

          for(auto curr : _conns) {
            FD_SET(curr.first.fd(), &socs);
            max = std::max(curr.first.fd(), max);
          }

          std::cout << "HI " << max << std::flush;
          if(select(max, &socs, NULL, NULL, NULL) == -1) {
            // TODO exception
            return;
          }
          std::cout << " BYE" << std::endl;

          if(FD_ISSET(_soc.fd(), &socs)) {
            Socket newsoc = _soc.Accept();

            _conns[newsoc] = std::make_shared<conn_t>(newsoc);

            std::cout << "Accepted new conn" << std::endl;
          }

          /*for(auto curr : _conns) {
            if(curr.first(is_set)) {
              std::cout << ""
            }
          }*/
        }

      }

    private:

      /** the connections that the server is serving */
      std::unordered_map<Socket, std::shared_ptr<conn_t> > _conns;

      uint16_t _port;  ///< the port that the server is listening on
      Socket   _soc;   ///< the socket that the server is accepting on
  };
}

