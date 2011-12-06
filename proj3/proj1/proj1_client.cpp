/*
 * client.cpp
 *
 *  Created on: Aug 28, 2011
 *      Author: mallal, norton, savage, sorensen
 */

#include <socket.hpp>
#include <common.hpp>

#include <signal.h>
#include <iostream>
#include <limits>
#include <cstring>
#include <string>
#include <sstream>
#include <getopt.h>
#include <cstdlib>

void timeout(int sig) {
  std::cerr << "Timeout on reply from server" << std::endl;
  exit(-1);
}

void usage(const string& exe) {
  std::cout << "usage: " << exe << " -t <tcp | udp> -s <server> -p <port> -x <num>" << std::endl;
  std::cout << "usage: all options are required" << std::endl;
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

  signal(SIGALRM, timeout);

  while((c = getopt(argc, argv, "x:t:s:p:")) > 0) {
    switch(c) {
      case 'x':
        istr.clear();
        istr.str(optarg);
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

  if(!x || !s || !p || !t) {
    usage(argv[0]);
  }

  net::sync_socket conn(server, port, tcp);
  message m(htonl(num));
  reply r;

  if(conn.send<message>(&m, sizeof(m)) != sizeof(m)) {
    perror("proj1_client: error of send() call");
    return -1;
  }

  alarm(3);

  if(conn.recv<reply>(&r, sizeof(r)) <= 0) {
    std::cerr << "proj1_client: error on recv() call" << std::endl;
    return -1;
  }

  return 0;
}


