/*
 * client.cpp
 *
 *  Created on: Aug 28, 2011
 *      Author: norton
 */

#include <socket.hpp>
#include <common.hpp>

#include <cstring>
#include <string>
using std::string;
#include <sstream>
using std::istringstream;

#include <getopt.h>

void usage(const string& exe) {
  exit(-1);
}

int main(int argc, char** argv) {
  int c, count = 0;
  unsigned int num;
  string server;
  string port;
  istringstream istr;
  bool tcp = false;
  bool x, s, p, t;
  x = s = p = t = false;

  while((c = getopt(argc, argv, "x:t:s:p:")) != 0) {
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
        port = optarg;
        p = true;
        break;
      default:
        usage(argv[0]);
        break;
    }
  }

  net::socket conn(server, port, tcp);
  message m(num);
  reply* r;

  conn.write<message>(m);
  r = conn.read<reply>();

  // THIS IS AWSOME!!!!!!!!
}


