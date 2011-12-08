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
#include <inttypes.h>
#include <stdio.h>

using namespace std;

const int debug = 1;


#define BUF_SIZE 8192
#define BIT(var ,shift) (var & (1 << shift))

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
		for(unsigned int k = 0;k<j.size();k++){
			q.QNAME[index++] = j[k];
			q.numBytes++;
		}
		q.numBytes++;
		
	}
	q.QTYPE=28;
	q.QCLASS=1;
	return q;
}

//kore wa nan desu ka?
void successParse(string response, uint16_t ANCOUNT){

}


//Sends the quiery and recieves the answer
string sendOut(string IP, string sendMe){
	//Set up the adderinfo struct
	struct addrinfo info, *reinfo;
	memset(&info, 0, sizeof info);
	info.ai_family = AF_INET;
	info.ai_socktype = SOCK_DGRAM;

	//TODO pack own struct!
	getaddrinfo(IP.c_str(), "53", &info, &reinfo);

	//set up the socket and connect
	//TODO error check here
	int sock = socket(reinfo->ai_family, reinfo->ai_socktype, reinfo->ai_protocol);
	connect(sock, reinfo->ai_addr, reinfo->ai_addrlen);

	int numSendBytes = sendMe.size();
	char head[BUF_SIZE];
        int numBytesRecv = 0;

	if (send(sock, (void *)sendMe.c_str(),numSendBytes,0) < numSendBytes){
		cerr << "Did not send the whole packet" << endl; 
	}
	if((numBytesRecv = recv(sock,head,BUF_SIZE-1,0)) == -1){
		cout << "FAILURE" << endl;			
	}
	stringstream results;
	results.write(head,numBytesRecv);
	return results.str();
}
/*
uint16_t atoi16(char val1,char val2){
		cout << "val1 " << val1 << endl;
		uint16_t value = val2 + (val1 << 8);
		return value;
}*/

uint16_t atoi16(string records,int & location){
	uint16_t value = records[location] + (records[location+1] << 8);
	location += 2;
	return ntohs(value);
}

uint16_t atoi32(string records,int & location){
	uint32_t value = records[location] + (records[location+1] << 8)
			+ (records[location+2] << 16)
			+ (records[location+3] << 24);
	location += 4;
	return ntohl(value);
}

string recParse(string & records, int & location, bool first=true ){
	uint8_t numchars = records[location];
	if(numchars == 0) return "";
	stringstream ss;
	if(BIT(records[location],6) && BIT(records[location],7)) {
		//if(debug) cout << "COMPRESSION FOUND: "<< location << endl;
		uint16_t ofst = atoi16(records, location)+1;
		//knock off the first 11... of a compressed record
		int offset = ofst & 63;
		//if(debug) cout << "OFFSET IS: "<< offset <<endl;
		return recParse(records, offset,first);
	}
	location++;
	int start = location;
	//cout << "NO COMPRESSION: " << location << " " << numchars << endl;
	for(location; location < start+numchars; location++){
		ss << records[location]; 
		
		//TODO something with ss;
	}
	if(first) return ss.str()+recParse(records,location,false);
	else return "."+ss.str()+recParse(records,location,first);
}

string ParseIP(string records){
	int numchars = records.size();
	stringstream ss;
	for(int i = 0; i < numchars;i++){
		if(i || i == numchars -1) ss <<  ".";
		uint8_t var = records[i];
		int var2 = var;
		ss << var2;
	}
	return ss.str();
}
vector<server> nextParse(string records,int location,uint16_t ANCOUNT, uint16_t NSCOUNT, uint16_t ARCOUNT){	
	vector<server> newList;
	for(int i = 0; i<ANCOUNT; i++){
		if(debug) cout << recParse(records,location) << endl;
		uint16_t TYPE = atoi16(records,location);
		uint16_t CLASS = atoi16(records,location);
	}
	for(int i = 0; i<NSCOUNT; i++){
		string NAME = recParse(records,location);
		if(debug) cout << "NAME: "<< NAME << endl;
		uint16_t TYPE = atoi16(records,location);
		if(debug) cout << "TYPE: "<< TYPE << endl;
		uint16_t CLASS = atoi16(records,location);
		if(debug) cout << "CLASS: "<< CLASS << endl;
		uint32_t TTL = atoi32(records,location);
		if(debug) cout << "TTL: "<< TTL << endl;
		uint16_t RDLENGTH = atoi16(records,location);
		if(debug) cout << "RDLENGTH: " << RDLENGTH << endl;
		location+=RDLENGTH;
	}
	for(int i = 0; i<ARCOUNT; i++) {
		cout << "Additional Records:" << endl;
	
		string NAME = recParse(records,location);
		if(debug) cout << "NAME: "<< NAME << endl;
		uint16_t TYPE = atoi16(records,location);
		if(debug) cout << "TYPE: "<< TYPE << endl;
		uint16_t CLASS = atoi16(records,location);
		if(debug) cout << "CLASS: "<< CLASS << endl;
		uint32_t TTL = atoi32(records,location);
		if(debug) cout << "TTL: "<< TTL << endl;
		uint16_t RDLENGTH = atoi16(records,location);
		if(debug) cout << "RDLENGTH: " << RDLENGTH << endl;
		string RDATA = ParseIP(records.substr(location,RDLENGTH));
		if(debug) cout << "RDATA: " << RDATA << endl;
		location+=RDLENGTH;
		server newServer = server(RDATA,NAME);
		if(RDLENGTH == 4) newList.push_back(newServer);
	}
	return newList;
}

vector<server> parse(string response, int qSize){
	if(debug) cout << response << endl;
	bool AA;
	AA =BIT((response[2]),2);
	if(AA){
		if(debug) cout << "AA IS TRUE" << endl;
	}
	if(debug) cout <<"RCODE before AND: " << response[3] << endl;
	uint8_t RCODE =response[3] & 15;
	if(debug) cout <<"RCODE after AND: "<< RCODE << endl;
	uint16_t ANCOUNT = response[7];
	uint16_t NSCOUNT = response[9];
	uint16_t ARCOUNT = response[11];
	if(debug) cout << ANCOUNT << " " << NSCOUNT << " " << ARCOUNT << endl;
	switch (RCODE)
	{
	case 0:
		// SUCCESS!
		if(debug) cout << "Question+Header size: "<< qSize<< endl;
		if(AA && ANCOUNT != 0){
			cout << "PARTY RIGHT!" << endl;
			exit(0);
			successParse(response,ANCOUNT);
		}else{
			return nextParse(response,qSize,ANCOUNT,NSCOUNT,ARCOUNT);
		}
		break;
	case 1: // Formatted request incorrectly.
		cout << "Incorrectly formatted request, exiting" <<endl;
		exit(1);
		break;
	case 2: // Server Screwed up. Try something else
		cout << "Server was unavalible. Trying another" <<endl;
		break;
	case 3: // Name doesn't exist, AN HERO.
		if(AA && ANCOUNT == 0){
			cout << "Sorry, no record found" << endl;
			exit(0);
		}
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
	vector<server> null;
	return null;	
}

/*
 * This method attepmts to connect to a random server from the list of servers
 *
 */
void attempt(vector<server> & servers,header pktHeader,question pktPayload){
	
	if(debug) cout << "attempting to connect to a listed server" << endl;
	srand(time(NULL));
	int Response = 0;

	while(!Response){
		if(debug) cout << "trying to find a random server out of: "<< servers.size() << endl;
		if(debug) cout << "RANDOM NUMBER TEST: "<<rand()<< endl;

		//Pick a random server to send
		int srvNum = rand() % servers.size();

		if(debug) cout << "sending to: " << servers[srvNum].getName() << endl;
		
		//This is the overall packet to send
		stringstream dnsPacket;

		//Formatting the differnt fields of the header
		uint16_t version = htons(pktHeader.version);
		uint16_t flags = htons(pktHeader.flags);
		uint16_t queries = htons(pktHeader.queries);
		uint16_t answers = htons(pktHeader.answers);
		uint16_t nservers = htons(pktHeader.nservers);
		uint16_t addservers = htons(pktHeader.addservers);

		//Add the formatted header fields to the packet
		dnsPacket.write((char*)&version,2);
		dnsPacket.write((char*)&flags,2);
                dnsPacket.write((char*)&queries,2);
     		dnsPacket.write((char*)&answers,2);
                dnsPacket.write((char*)&nservers,2);
                dnsPacket.write((char*)&addservers,2);

		//Format and add the DNS quiery data to the packet
		dnsPacket.write(pktPayload.QNAME,pktPayload.numBytes);
		uint8_t stopper = 0;
		uint16_t qtype = htons(pktPayload.QTYPE);
		uint16_t qclass = htons(pktPayload.QCLASS);
		dnsPacket.write((char*)&stopper,1);
		dnsPacket.write((char*)&qtype,2);
		dnsPacket.write((char*)&qclass,2);
		int numbytes = dnsPacket.str().size();
		//send the packet
		string results = sendOut(servers[srvNum].getIP(),dnsPacket.str());

		//if we get an answer, extract the information
		vector<server> newServerList = parse(results,numbytes);
		if(!newServerList.empty()){
			attempt(newServerList,pktHeader,pktPayload);

		}
		//if we dont' get an answer, timeout amd pick another server from the list
		servers.erase(servers.begin()+srvNum);
		
		//if there are no servers, then no answer found
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
