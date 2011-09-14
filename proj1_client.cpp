/*
 * client.cpp
 *
 *  Created on: Aug 28, 2011
 *      Author: norton
 */

#include <socket.hpp>
#include <common.hpp>

#include <iostream>
#include <limits>
#include <cstring>
#include <string>
#include <sstream>
#include <getopt.h>
#include <cstdlib>


void usage(const string& exe) {
  std::cout << "usage: " << exe << std::endl;
  exit(-1);
}

int main(int argc, char** argv) {
  unsigned int num = 0;
  std::string server;
  std::string port;
  std::istringstream istr;
  bool tcp = false;
  bool x, s, p, t;
  int c;
  x = s = p = t = false;

  while((c = getopt(argc, argv, "x:t:s:p:")) > 0) {
    switch(c) {
      case 'x':
        istr.str() = optarg;
        istr >> num;
        x = true;
        break;
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
      case 's':
        server = optarg;
        s = true;
        break;
      case 'p':
        p = true;
        port = optarg;
        c = atoi(port.c_str());

        if(c < 0 || c > std::numeric_limits<unsigned short>::max()) {
          //TODO error
        }

        break;
      default:
        usage(argv[0]);
        break;
    }
  }

  std::cout << server << " " << port << " " << tcp << std::endl;
  net::sync_socket conn(server, port, tcp);
  message m(num);
  reply r;

  conn.send(&m, sizeof(m));
  conn.recv(&r, sizeof(r));

  return 0;
}


