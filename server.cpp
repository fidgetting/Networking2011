/*
 * server.cpp
 *
 *  Created on: Aug 29, 2011
 *      Author: mallal
 */

//TODO just testing git adding and stuff

#include <getopt.h>

#define BACKLOG 10

//TODO
void usage(const string& exe) {
	exit(-1);
}

int main(int argc, char** argv) {
  bool tcp = false;
	bool t, p;
    t = p = false;

  while((c = getopt(argc, argv, "t:p:")) != 0) {
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
          port = optarg;
          p = true;
          break;
       default:
          usage(argv[0]);
          break;
      }
  }
}
