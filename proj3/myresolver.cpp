/*
 * common.hpp
 *
 *  Created on: Dec 1, 2011
 *      Author: mallal, norton, savage, sorensen
 */

/* local includes */
#include <common.hpp>

/* unix includes */
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <inttypes.h>
#include <arpa/inet.h>

/* c std library includes */
#include <algorithm>
#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

/* boost includes */
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/ostream_iterator.hpp>
namespace it = boost::archive::iterators;
/* ************************************************************************** */
/* *** Utility functions and defines **************************************** */
/* ************************************************************************** */

#ifdef DEBUG
#define DEBUG_PRINT(str) std::cout << str << std::endl
#else
#define DEBUG_PRINT(str)
#endif

#define FATAL(str) { \
		std::cerr << "FATAL: " << __FILE__ << "." << __LINE__ << ": " << str << std::endl; \
		exit(-1); }

#define BUF_SIZE 8192
#define BIT(var ,shift) (var & (1 << shift))

#define IPV4_LEN 4
#define IPV6_LEN 16

std::string query_name;

uint8_t atoi8(const std::string& records, uint32_t& location) {
  return (int)static_cast<uint8_t>(records[location++]);
}

uint16_t atoi16(const std::string& records, uint32_t& location) {
  uint16_t value = records[location++]
                           + (records[location++] << 8);
  return ntohs(value);
}

uint32_t atoi32(const std::string& records, uint32_t& location) {
  uint32_t value;

  memcpy(&value, records.c_str() + location, 4);
  location += 4;

  return ntohl(value);
}

enum RCODE_t {
  SUCCESS = 0,
  FORMAT  = 1,
  SERVER  = 2,
  NAME    = 3,
  NOT_IMP = 4,
  REFUSED = 5
};

enum TYPE_t {
  A     = 1,
  NS    = 2,
  MD    = 3,
  MF    = 4,
  CNAME = 5,
  SOA   = 6,
  MB    = 7,
  MG    = 8,
  MR    = 9,
  null  = 10,
  WKS   = 11,
  PTR   = 12,
  HINFO = 13,
  MINFO = 14,
  MX    = 15,
  TXT   = 16,
  AAAA  = 28,
  RRSIG = 46
};

enum CLASS_t {
  IN = 1,
  CS = 2,
  CH = 3,
  HS = 4
};

std::map<uint32_t, std::string> TYPE_t_strings;
std::map<uint32_t, std::string> CLASS_t_strings;

/* ************************************************************************** */
/* *** dns server information *********************************************** */
/* ************************************************************************** */

class server{
  public:
    server(const std::string& IP, const std::string& name) :
      _IP(IP), _name(name) { }

    inline const std::string&  name() const { return _name; }
    inline const std::string&    IP() const { return _IP;   }

  private:
    std::string _IP;
    std::string _name;
};

bool operator==(const server& l, const server& r) {
  return l.name() == r.name() && l.IP() == r.IP();
}

std::vector<server> initialize(){
  std::vector<server> vServers;

  TYPE_t_strings[1] = "A";
  TYPE_t_strings[2] = "NS";
  TYPE_t_strings[3] = "MD";
  TYPE_t_strings[4] = "MF";
  TYPE_t_strings[5] = "CNAME";
  TYPE_t_strings[6] = "SOA";
  TYPE_t_strings[7] = "MB";
  TYPE_t_strings[8] = "MG";
  TYPE_t_strings[9] = "MR";
  TYPE_t_strings[10] = "null";
  TYPE_t_strings[11] = "WKS";
  TYPE_t_strings[12] = "PTR";
  TYPE_t_strings[13] = "HINFO";
  TYPE_t_strings[14] = "MINFO";
  TYPE_t_strings[15] = "MX";
  TYPE_t_strings[16] = "TXT";
  TYPE_t_strings[28] = "AAAA";
  TYPE_t_strings[46] = "RRSIG";

  CLASS_t_strings[1] = "IN";
  CLASS_t_strings[2] = "CS";
  CLASS_t_strings[3] = "CH";
  CLASS_t_strings[4] = "HS";

  vServers.push_back(server("198.41.0.4",     "ns.internic.net" ));
  vServers.push_back(server("192.228.79.201", "ns1.isi.edu"     ));
  vServers.push_back(server("192.33.4.12",    "c.psi.net"       ));
  vServers.push_back(server("128.8.10.90",    "terp.umd.edu"    ));
  vServers.push_back(server("192.203.230.10", "ns.nasa.gov"     ));
  vServers.push_back(server("192.5.5.241",    "ns.isc.org"      ));
  vServers.push_back(server("192.112.36.4",   "ns.nic.ddn.mil"  ));
  vServers.push_back(server("128.63.2.53",    "aos.arl.army.mil"));
  vServers.push_back(server("192.36.148.17",  "nic.nordu.net"   ));
  vServers.push_back(server("192.58.128.30",  "Verisign"        ));
  vServers.push_back(server("193.0.14.129",   "RIPE NCC"        ));
  vServers.push_back(server("199.7.83.42",    "ICANN"           ));
  vServers.push_back(server("202.12.27.33",   "WIDE Project"    ));
  return vServers;
}

struct dns_response {
    std::string Name;
    uint32_t TimeToLive;
    uint16_t Class;
    uint16_t Type;
    struct sockaddr_in6 Addr;
    std::string rrsig;
};

/* ************************************************************************** */
/* *** function delcarations ************************************************ */
/* ************************************************************************** */

void makeHeader(header&);
void makeQuestion(std::string, question&);

std::string sendOut(const std::string&, const std::string&);
std::string recParse(const std::string&, uint32_t&, bool = true);
std::string ParseIP(const std::string&, bool);
std::vector<server> nextParse(const std::string&, uint32_t, uint16_t, uint16_t, uint16_t);
std::vector<server> parse(std::string, uint32_t);
void attempt(std::vector<server>&, const header&, const question&, std::vector<server>&);
void successParse(std::string, uint32_t, uint16_t);
std::string parseRRSIG(std::string, uint32_t&);

/* ************************************************************************** */
/* *** TODO ***************************************************************** */
/* ************************************************************************** */

void makeHeader(header& q) {
  DEBUG_PRINT("packing query");
  q.version = 0;
  q.flags = 0;
  q.queries = 1;
  q.answers = 0;
  q.nservers = 0;
  q.addservers = 1;
  DEBUG_PRINT("packing finished");
}

void makeQuestion(std::string name, question& q) {
  std::stringstream ss;
  std::string j;
  int index = 0;
  uint8_t numChar;

  DEBUG_PRINT(name);
  for(unsigned int i= 0;i< name.size();i++){
    if(name[i] == '.'){
      name[i] = ' ';
    }
  }
  DEBUG_PRINT(name);
  ss << name;

  while(ss >> j){
    numChar = j.size();
    q.QNAME[index++] = numChar;
    for(unsigned int k = 0;k<j.size();k++){
      q.QNAME[index++] = j[k];
      q.numBytes++;
    }
    q.numBytes++;
  }

  q.QTYPE  = 28;
  q.QCLASS = 1;

  /* cludge */
  q.QODD[0] = q.QODD[1] = 0;
  q.QODD[2] = 41;
  q.QODD[3] = 16;
  q.QODD[4] = q.QODD[5] = q.QODD[6] = 0;
  q.QODD[7] = 128;
  q.QODD[8] = q.QODD[9] = q.QODD[10] = 0;
}

/**
 * Sends the intial dns query and recieves the response
 *
 * @param IP the ip address of the server
 * @param sendMe the string query
 * @return the string recieved from the server
 */
std::string sendOut(const std::string& IP, const std::string& sendMe) {
  std::streamsize numBytesRecv = 0;
  struct addrinfo info, *res;
  uint32_t sock;
  uint32_t numSendBytes = sendMe.size();
  char head[BUF_SIZE];

  /* get the networking info for dns server */
  memset(&info, 0, sizeof info);
  info.ai_family = AF_INET;
  info.ai_socktype = SOCK_DGRAM;
  getaddrinfo(IP.c_str(), "53", &info, &res);

  //set up the socket and connect
  if((sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1)
    FATAL("socket call failed for dns server connection: " << strerror(errno));
  if(connect(sock, res->ai_addr, res->ai_addrlen) == -1)
    FATAL("unable to connect to the dns server");
  if (send(sock, (void *)sendMe.c_str(), numSendBytes,0) < numSendBytes)
    FATAL("unable to send query to dns server");
  if((numBytesRecv = recv(sock, head, BUF_SIZE-1, 0)) == -1)
    FATAL("failed receiving from dns server");

  std::stringstream results;
  results.write(head, numBytesRecv);
  return results.str();
}

/**
 * TODO
 *
 * @param records
 * @param location
 * @param first
 * @return
 */
std::string recParse(const std::string & records, uint32_t& location, bool first) {
  std::stringstream ss;
  uint8_t  numchars = records[location];
  uint32_t start;

  if(numchars == 0)
    return "";

  if(BIT(records[location], 7) && BIT(records[location], 6)) {
    //knock off the first 11... of a compressed record
    start = (atoi16(records, location) + 1) & 63;
    return recParse(records, start, first);
  }

  start = (++location);
  while(location < start+numchars){
    ss << records[location];
    location++;
  }

  return first ?
      ss.str() + recParse(records, location, false):
      "." + ss.str() + recParse(records, location, first);
}

/**
 * TODO
 *
 * @param records
 * @return
 */
std::string ParseIP(const std::string& records, bool ipv6) {
  int numchars = records.size();
  std::stringstream ss;

  if(ipv6) {
    for(int i = 0; i < numchars; i++) {
      if((i || i == numchars - 1) && i % 2 == 0)
        ss << ":";
      ss << std::setw(2) << std::setfill('0') << std::hex
         << (int)static_cast<uint8_t>(records[i]);
    }

    return ss.str();
  }

  for(int i = 0; i < numchars; i++) {
    if(i || i == numchars - 1)
      ss <<  ".";
    ss << (int)static_cast<uint8_t>(records[i]);
  }

  return ss.str();
}

/**
 * TODO
 *
 * @param records
 * @param location
 * @param ANCOUNT
 * @param NSCOUNT
 * @param ARCOUNT
 * @return
 */
std::vector<server> nextParse(const std::string& records, uint32_t location, uint16_t ANCOUNT, uint16_t NSCOUNT, uint16_t ARCOUNT) {
  std::vector<server> newList;
  std::string NAME;
  std::string RDATA;
  uint16_t TYPE;
  uint16_t CLASS;
  uint32_t TTL;
  uint16_t RDLENGTH;

  DEBUG_PRINT("Name Server Records:");
  for(int i = 0; i < ANCOUNT; i++){
    NAME     = recParse(records, location);
    TYPE     = atoi16(records, location);
    CLASS    = atoi16(records, location);
    TTL      = atoi32(records, location);
    RDLENGTH = atoi16(records, location);
    location += RDLENGTH;

    DEBUG_PRINT("  ENTRY: " << i);
    DEBUG_PRINT("    TYPE:     " << TYPE    );
    DEBUG_PRINT("    CLASS:    " << CLASS   );
    DEBUG_PRINT("    TTL:      " << TTL     );
    DEBUG_PRINT("    RDLENGTH: " << RDLENGTH);
  }

  DEBUG_PRINT("Name Server Records:");
  for(int i = 0; i < NSCOUNT; i++){
    NAME     = recParse(records, location);
    TYPE     = atoi16(records, location);
    CLASS    = atoi16(records, location);
    TTL      = atoi32(records, location);
    RDLENGTH = atoi16(records, location);
    location += RDLENGTH;

    DEBUG_PRINT("  ENTRY: " << i);
    DEBUG_PRINT("    TYPE:     " << TYPE    );
    DEBUG_PRINT("    CLASS:    " << CLASS   );
    DEBUG_PRINT("    TTL:      " << TTL     );
    DEBUG_PRINT("    RDLENGTH: " << RDLENGTH);
  }

  DEBUG_PRINT("Additional Records:");
  for(int i = 0; i < ARCOUNT; i++) {
    NAME     = recParse(records, location);
    TYPE     = atoi16(records, location);
    CLASS    = atoi16(records, location);
    TTL      = atoi32(records, location);
    RDLENGTH = atoi16(records, location);
    RDATA    = ParseIP(records.substr(location, RDLENGTH), RDLENGTH == IPV6_LEN);
    location += RDLENGTH;

    DEBUG_PRINT("  ENTRY: " << i);
    DEBUG_PRINT("    TYPE:     " << TYPE    );
    DEBUG_PRINT("    CLASS:    " << CLASS   );
    DEBUG_PRINT("    TTL:      " << TTL     );
    DEBUG_PRINT("    RDLENGTH: " << RDLENGTH);
    DEBUG_PRINT("    IP_ADDR:  " << RDATA   );

    if(RDLENGTH == 4) {
      newList.push_back(server(RDATA,NAME));
    }
  }

  for(int i = 0; i < newList.size(); i++) {
    DEBUG_PRINT(newList[i].IP() << " " << newList[i].name());
  }

  return newList;
}

/**
 * TODO???
 *
 * @param response
 * @param ANCOUNT
 */
void successParse(std::string records, uint32_t location, uint16_t ANCOUNT) {
  std::vector<dns_response> responses;
  std::string RDATA;
  dns_response resp;
  uint16_t RDLENGTH;
  char buf[INET6_ADDRSTRLEN];

  resp.Name       = recParse(records, location);
  resp.Type       = atoi16(records, location);
  resp.Class      = atoi16(records, location);
  resp.TimeToLive = atoi32(records, location);
  RDLENGTH        = atoi16(records, location);
  RDATA           = ParseIP(records.substr(location, RDLENGTH), RDLENGTH == IPV6_LEN);
  location += RDLENGTH;
  resp.rrsig = parseRRSIG(records, location);

  DEBUG_PRINT("Additional Records:");
  DEBUG_PRINT("  NAME:     " << resp.Name       );
  DEBUG_PRINT("  TYPE:     " << resp.Type       );
  DEBUG_PRINT("  CLASS:    " << resp.Class      );
  DEBUG_PRINT("  TTL:      " << resp.TimeToLive );
  DEBUG_PRINT("  RDLENGTH: " << RDLENGTH        );
  DEBUG_PRINT("  RDATA:    " << RDATA           );

  if(RDLENGTH == IPV6_LEN) {
    inet_pton(AF_INET6, RDATA.c_str(), &(resp.Addr.sin6_addr));
    responses.push_back(resp);
  }

  std::cout << responses.size() << " IPv6 Address Found For "
            << query_name << ":\n" << std::endl;
  for(std::vector<dns_response>::iterator iter = responses.begin();
      iter != responses.end(); iter++) {
    inet_ntop(AF_INET6, &(iter->Addr.sin6_addr), buf, INET6_ADDRSTRLEN);

    std::cout << iter->Name << " " << iter->TimeToLive << " "
        << CLASS_t_strings[iter->Class] << " " << TYPE_t_strings[iter->Type]
        << " " << buf << std::endl;
    std::cout << iter->rrsig << std::endl;
  }

  exit(0);
}

/**
 * TODO
 *
 * @param records
 * @param location
 */
std::string parseRRSIG(std::string records, uint32_t& location) {
  typedef it::insert_linebreaks<it::base64_from_binary<it::transform_width<const char*, 6, 8> >, 72> base64_text;

  std::stringstream os;
  std::ostringstream ostr;
  std::string NAME;
  std::string S_NAME;
  std::string SIGNATURE;
  uint8_t  ALGOR;
  uint8_t  LABELS;
  uint16_t TYPE;
  uint16_t TYPE_COV;
  uint16_t CLASS;
  uint16_t RDLENGTH;
  uint16_t SIGN_ID;
  uint32_t TTL;
  uint32_t ORG_TTL;
  uint32_t SIG_EXP;
  uint32_t T_SIGN;

  NAME     = recParse(records, location);
  TYPE     = atoi16  (records, location);
  CLASS    = atoi16  (records, location);
  TTL      = atoi32  (records, location);
  RDLENGTH = atoi16  (records, location);
  TYPE_COV = atoi16  (records, location);
  ALGOR    = atoi8   (records, location);
  LABELS   = atoi8   (records, location);
  ORG_TTL  = atoi32  (records, location);
  SIG_EXP  = atoi32  (records, location);
  T_SIGN   = atoi32  (records, location);
  SIGN_ID  = atoi16  (records, location);
  S_NAME   = recParse(records, location);
  location++;
  SIGNATURE = records.substr(location, RDLENGTH - 20 - S_NAME.length());

  std::copy(base64_text(SIGNATURE.c_str()), base64_text(SIGNATURE.c_str() + SIGNATURE.size()),
      it::ostream_iterator<char>(os));

  ostr << NAME << " " << TTL << " " << CLASS_t_strings[CLASS] << " "
       << TYPE_t_strings[TYPE] << " " << TYPE_t_strings[TYPE_COV] << " "
       << int(ALGOR) << " " << int(LABELS) << " " << ORG_TTL << " " << SIG_EXP
       << " " << T_SIGN << " " << SIGN_ID << " " << S_NAME << " "
       << os.str() << "=" << std::endl;

  return ostr.str();
}

/**
 * TODO
 *
 * @param response
 * @param qSize
 * @return
 */
std::vector<server> parse(std::string response, uint32_t qSize) {
  uint8_t RCODE;
  uint16_t ANCOUNT;
  uint16_t NSCOUNT;
  uint16_t ARCOUNT;
  bool AA;

  DEBUG_PRINT(response);
  if((AA = BIT((response[2]),2))){
    DEBUG_PRINT("AA is true");

    // TODO other stuff
  }

  DEBUG_PRINT("RCODE before AND: " << response[3]);

  RCODE   = response[3] & 15;
  ANCOUNT = response[7];
  NSCOUNT = response[9];
  ARCOUNT = response[11];

  DEBUG_PRINT("RCODE after AND: "<< RCODE);
  DEBUG_PRINT(ANCOUNT << " " << NSCOUNT << " " << ARCOUNT);

  switch (RCODE)
  {
    case SUCCESS:
      DEBUG_PRINT("Question + Header size: " << qSize);
      if(AA && ANCOUNT != 0){
        DEBUG_PRINT("It worked");
        successParse(response, qSize, ANCOUNT);
      }else{
        return nextParse(response, qSize, ANCOUNT, NSCOUNT, ARCOUNT);
      }
      break;

    case FORMAT: // Formatted request incorrectly.
      FATAL("Incorrectly formatted request, exiting");
      break;

    case SERVER: // Server Screwed up. Try something else
      DEBUG_PRINT("Server was unavailable. Trying another");
      break;

    case NAME: // Name doesn't exist, AN HERO.
      if(AA && ANCOUNT == 0){
        std::cout << "Sorry, no record found" << std::endl;
        exit(0);
      }
      break;

    case NOT_IMP: // not implemented? WTF???
      DEBUG_PRINT("Server doesn't run IPV6 DNS? Retrying");
      break;

    case REFUSED: // Server Refused
      DEBUG_PRINT("Server refuses to fulfill request.");
      break;

    default: // Server apparently doesn't follow the RFC? WTF???
      DEBUG_PRINT("Server responded with an unknown RCODE.");
      break;
  }

  return std::vector<server>();
}

/**
 * Attempts to connect to dns server
 *
 * @param servers
 * @param pktHeader
 * @param pktPayload
 */
void attempt(std::vector<server>& servers, const header& pktHeader, const question& pktPayload, std::vector<server>& stack) {
  uint16_t version, flags, queries, answers, nservers, addservers, qtype, qclass;
  std::vector<server> newServerList;
  std::stringstream dnsPacket;
  std::string results;
  uint32_t Response = 0;
  uint32_t srvNum;
  uint32_t numbytes;
  uint8_t  stopper;

  DEBUG_PRINT("attempting to connect to a listed server");

  while(!Response && !servers.empty()) {
    DEBUG_PRINT("trying to find a random server out of: "<< servers.size());

    //Pick a random server to send
    srvNum  = rand() % servers.size();
    stopper = 0;

    DEBUG_PRINT("sending to: " << servers[srvNum].name());

    if(std::find(stack.begin(), stack.end(), servers[srvNum]) != stack.end()) {
      servers.erase(std::find(servers.begin(), servers.end(), servers[srvNum]));
      continue;
    }

    stack.push_back(servers[srvNum]);

    //Formatting the differnt fields of the header
    version    = htons(pktHeader.version);
    flags      = htons(pktHeader.flags);
    queries    = htons(pktHeader.queries);
    answers    = htons(pktHeader.answers);
    nservers   = htons(pktHeader.nservers);
    addservers = htons(pktHeader.addservers);

    //Add the formatted header fields to the packet
    dnsPacket.write((char*)&version,   2);
    dnsPacket.write((char*)&flags,     2);
    dnsPacket.write((char*)&queries,   2);
    dnsPacket.write((char*)&answers,   2);
    dnsPacket.write((char*)&nservers,  2);
    dnsPacket.write((char*)&addservers,2);

    //Format and add the DNS quiery data to the packet
    qtype  = htons(pktPayload.QTYPE);
    qclass = htons(pktPayload.QCLASS);
    dnsPacket.write(pktPayload.QNAME,pktPayload.numBytes);
    dnsPacket.write((char*)&stopper,1);
    dnsPacket.write((char*)&qtype,2);
    dnsPacket.write((char*)&qclass,2);
    dnsPacket.write((char*)pktPayload.QODD, 11);
    numbytes = dnsPacket.str().size() - 11;

    //send the packet
    results = sendOut(servers[srvNum].IP(), dnsPacket.str());

    //if we get an answer, extract the information
    newServerList = parse(results, numbytes);
    if(!newServerList.empty()){
      attempt(newServerList, pktHeader, pktPayload, stack);
    }

    //if we dont' get an answer, timeout amd pick another server from the list
    servers.erase(servers.begin()+srvNum);
    stack.pop_back();
  }

  //if there are no servers, then no answer found
  if(servers.empty()){
    std::cout << "No Record avalible" << std::endl;
    exit(1);
  }
}

/* ************************************************************************** */
/* *** main types *********************************************************** */
/* ************************************************************************** */

int usage(){
  std::cout << "Usage: myresolver hostname" << std::endl;
  exit(0);
}

int main(int argc,char *argv[]) {
  std::vector<server> hostServers;
  std::vector<server> servers;
  std::vector<server> stack;
  header qName;
  question hName;
  bool bFound, bNextPack;

  /* starting intializations */
  if(argc != 2) usage();
  hostServers = initialize();
  servers = hostServers;
  srand(time(NULL));
  bFound    = false;
  bNextPack = false;

  DEBUG_PRINT("Number of Host Servers Loaded: " << hostServers.size());
  DEBUG_PRINT("site: " << argv[1]);

  query_name = argv[1];
  makeHeader(qName);
  makeQuestion(query_name, hName);

  while(!bFound){
    while(!bNextPack){
      attempt(hostServers, qName, hName, stack);
    }
  }

  return 0;
}
