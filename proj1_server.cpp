/*
 * server.cpp
 *
 *  Created on: Aug 29, 2011
 *      Author: mallal, norton, savage, sorenson
 */

#include <service.hpp>
#include <common.hpp>

#include <signal.h>
#include <getopt.h>
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <limits>

//TODO
void usage(const string& exe) {
  std::cout << "usage: " << exe << " -t <tcp|udp> -p <port #>" << std::endl;
  exit(-1);
}

bool receive_tcp(net::sync_socket& soc) {
  message msg;
  reply rp;

  soc.recv(&msg, sizeof(msg));
  std::cout << msg.num << std::endl;
  soc.send(&rp, sizeof(rp));
  return false;
}

bool receive_udp(int fd) {
  message msg;
  reply rp;
  struct sockaddr addr;
  socklen_t addr_len;

  recvfrom(fd, &msg, sizeof(msg), 0, &addr, &addr_len);
  std::cout << msg.num << std::endl;
  sendto(fd, &rp, sizeof(rp), 0, &addr, addr_len);

  return false;
}

int main(int argc, char** argv) {
  bool tcp = false;
  bool t, p;
  t = p = false;
  int port, c;
  net::sync_service svc;

  port = -1;

  while((c = getopt(argc, argv, "t:p:")) > 0) {
    switch(c) {
      case 't':
        t = true;
        if(strcmp(optarg, "udp") == 0)
          break;
        if(strcmp(optarg, "tcp") == 0) {
          tcp = true;
          break;
        }
        usage(argv[0]);
        break;
      case 'p':
        p = true;
        port = std::atoi(optarg);
        if(port < 0 || port > std::numeric_limits<unsigned short>::max()) {
          throw std::exception();
        }
        break;
      default:
        std::cout << c << std::endl;
        usage(argv[0]);
        break;
    }
  }

  svc.add_port(port,tcp);
  svc.listen(receive_tcp, receive_udp);

  return 0;
}
