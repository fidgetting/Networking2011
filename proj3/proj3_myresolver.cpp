#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
using namespace std;

class server{
     public:
    	string IP;
    	string name;
     	server(string server,string IP);
	char* getName();
        char* getIP();
};

server::server(string sServer,string sIP){
	IP = sIP;
        name = sServer;
}

char* server::getIP(){
return IP.c_str();
}

char* server::getName(){
return IP.c_str();
}

vector<server> initialize(){
	vector<server> vServers;
	vServers.insert(server("198.41.0.4","ns.internic.net");
	return vServers;
}

int usage(){
	cout << "Usage: myresolver hostname\n";
	exit(0);
}

main(int argc,char *argv[])
{
	vector<server> hostServers  = initialize();
	if(argc != 2){
		usage();
	}
	return 0;
}
