#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <sstream>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <socket.hpp>
#include <common.hpp>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>


using namespace std;

const int debug = 1;


#define BUF_SIZE 8192

class server{
     private:
    	string IP;
    	string name;
     public:
     	server(string server,string IP);
	string getName();
	string getIP();
};

server::server(string sIP,string sServer){
	IP = sIP;
        name = sServer;
}

string server::getIP(){
       	return IP;
}

string server::getName(){
	return name;
}



vector<server> initialize(){
	vector<server> vServers;
	vServers.push_back(server("198.41.0.4","ns.internic.net"));
	vServers.push_back(server("192.228.79.201","ns1.isi.edu"));
	vServers.push_back(server("192.33.4.12","c.psi.net"));
	vServers.push_back(server("128.8.10.90","terp.umd.edu"));
	vServers.push_back(server("192.203.230.10","ns.nasa.gov"));
	vServers.push_back(server("192.5.5.241","ns.isc.org"));
	vServers.push_back(server("192.112.36.4","ns.nic.ddn.mil"));
	vServers.push_back(server("128.63.2.53","aos.arl.army.mil"));
	vServers.push_back(server("192.36.148.17","nic.nordu.net"));
	vServers.push_back(server("192.58.128.30","Verisign"));
	vServers.push_back(server("193.0.14.129","RIPE NCC"));
	vServers.push_back(server("199.7.83.42","ICANN"));
	vServers.push_back(server("202.12.27.33","WIDE Project"));
	return vServers;
}

int usage(){
	cout << "Usage: myresolver hostname\n";
	exit(0);
}

header makeHeader(){
	if(debug) cout << "packing query" << endl;
	header q;
	q.version = 0;
        q.flags = 0;
        q.queries = 1;
        q.answers = 0;
        q.nservers = 0;
        q.addservers = 0;
        if(debug) cout << "packing finished" << endl;
	return q;
	
}

question makeQuestion(string name){
	question q;
	cout << name << endl;
	for(unsigned int i= 0;i< name.size();i++){
              if(name[i] == '.'){
                      name[i] = ' ';
              }
        }
        cout << name << endl;
	stringstream ss;
	ss << name;

	string j;
	int index = 0;
	while(ss >> j){
		uint8_t numChar = j.size();
		q.QNAME[index++] = numChar;
		for(int k = 0;k<j.size();k++){
			q.QNAME[index++] = j[k];
			q.numBytes++;
		}
		q.numBytes++;
		
	}
	q.QTYPE=28;
	q.QCLASS=1;
	return q;
}

void successParse(string response){

}

string sendOut(string IP, string sendMe, bool TCP){
	struct addrinfo info, *reinfo;
	memset(&info, 0, sizeof info);
	info.ai_family = AF_INET;
	if(TCP){
		info.ai_socktype = SOCK_STREAM;
	}
	else info.ai_socktype = SOCK_DGRAM;
	getaddrinfo(IP.c_str(), "53", &info, &reinfo);
	cout << "Things" << endl;
	int sock = socket(reinfo->ai_family, reinfo->ai_socktype, reinfo->ai_protocol);
	if(TCP){
		connect(sock, reinfo->ai_addr, reinfo->ai_addrlen);
		cout << "PORT:" << sock << endl;
		int derp = sendMe.size();
		int herp = send(sock, (void *)sendMe.c_str(),derp,0);
		char rec[BUF_SIZE];
		int numbyts = 0;
		while(numbyts == 0){
			cout <<"PORT: "<< sock << endl;
			if((numbyts = recv(sock,rec,BUF_SIZE-1,0)) == -1){
				cout << "FAILURE" << endl;
			}
			cout << numbyts << endl;
		}
//		cout << "WE WILL NOT FAIL" << endl;
		cout << numbyts << endl;
		stringstream mf;
		mf.write(rec,numbyts);
		cout << mf.str() << endl;
		cout << mf.str().size() << endl;
	}else{
		connect(sock, reinfo->ai_addr, reinfo->ai_addrlen);
		cout << "PORT:" << sock << endl;
		int derp = sendMe.size();
		int herp = send(sock, (void *)sendMe.c_str(),derp,0);
		char head[BUF_SIZE];
		char body[BUF_SIZE];
		int hNumbyts = 0;
		while(hNumbyts == 0){
			if((hNumbyts = recv(sock,head,BUF_SIZE-1,0)) == -1){
				cout << "FAILURE" << endl;
			}
		}
//		cout << "WE WILL NOT FAIL" << endl;
		stringstream mf;
		mf.write(head,hNumbyts);
		cout << mf.str() << endl;
		cout << mf.str().size() << endl;
	}
	return "";
}


void parse(string response){
	bool AA;
	AA =(response[3] == (char)(132));
	uint8_t RCODE;
	
	switch (RCODE)
	{
	case 0:
		// SUCCESS!
//		return successParse(response);
		break;
	case 1: // Formatted request incorrectly.
		cout << "Incorrectly formatted request, exiting" <<endl;
		exit(1);
		break;
	case 2: // Server Screwed up. Try something else
		cout << "Server was unavalible. Trying another" <<endl;
		break;
	case 3: // Name doesn't exist, AN HERO.
		cout << "Sorry, no record found" << endl;
		exit(0);
		break;
	case 4: // not imprlemented? WTF.
		cout << "Server doesn't run IPV6 DNS? Retrying" << endl;
		break;
	case 5: // Server Refused
		cout << "Server refuses to fufill request." << endl;
		break;
	default: // Server apperently doesn't follow the RFC. WTF.
		cout << "Server responded with an unknown RCODE." << endl;
		break;
	}
//	return NULL;	
}

void attempt(vector<server> & servers,header toSend,question toSend2){
	
	cout << "attempting to connect to a listed server" << endl;
	srand(time(NULL));
	int Response = 0;
	while(!Response){
		if(debug) cout << "trying to find a random server out of: "<< servers.size() << endl;
		if(debug) cout << "RANDOM NUMBER TEST: "<<rand()<< endl;
		int srvNum = rand() % servers.size();
		if(debug) cout << "sending to: " << servers[srvNum].getName() << endl;
		net::sync_socket conn(servers[srvNum].getIP(),"53", false);
		stringstream toSendAway;
		uint16_t version = htons(toSend.version);
		uint16_t flags = htons(toSend.flags);
		uint16_t queries = htons(toSend.queries);
		uint16_t answers = htons(toSend.answers);
		uint16_t nservers = htons(toSend.nservers);
		uint16_t addservers = htons(toSend.addservers);
		toSendAway.write((char*)&version,2);
		toSendAway.write((char*)&flags,2);
                toSendAway.write((char*)&queries,2);
     		toSendAway.write((char*)&answers,2);
                toSendAway.write((char*)&nservers,2);
                toSendAway.write((char*)&addservers,2);
		toSendAway.write(toSend2.QNAME,toSend2.numBytes);
		uint8_t stopper = 0;
		uint16_t qtype = htons(toSend2.QTYPE);
		uint16_t qclass = htons(toSend2.QCLASS);
		toSendAway.write((char*)&stopper,1);
		toSendAway.write((char*)&qtype,2);
		toSendAway.write((char*)&qclass,2);
		sendOut(servers[srvNum].getIP(),toSendAway.str(),false);
		//if(conn.send<char>(toSendAway.str().c_str(), toSendAway.str().size())){
		//	perror("myresolver: error sending message");
		//}
		char answer[BUF_SIZE];
		memset(&answer, '\0', sizeof(answer));
		alarm(3);
		
		if(debug) cout << "sent\ntrying to recieve" << endl;
		  if(conn.recv<char>(answer, sizeof(answer)) <= 0) {
		    std::cerr << "nothing recieved bitchez" << endl;
  		}
  		cout << answer << endl;

		servers.erase(servers.begin()+srvNum);
		if(servers.size() == 0){
			cout<<"No Record avalible"<<endl;
			exit(1);
		}	
	}

}

int main(int argc,char *argv[])
{
	vector<server> hostServers  = initialize();
	if(argc != 2){
		usage();
	}
	if(debug){
		cout << "Number of Host Servers Loaded: " <<
		 hostServers.size() << endl;
		cout << "site: "<< argv[1] << endl;
	}
	stringstream ss;
	ss << argv[1];
	header qName = makeHeader();
	question hName = makeQuestion(ss.str());
	int bFound = 0,bNextPack = 0;
	vector<server> servers = hostServers;
	while(!bFound){
		
		while(!bNextPack){
			attempt(servers, qName, hName);
		}
	}

	return 0;
}