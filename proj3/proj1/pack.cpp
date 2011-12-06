#include <vector>
#include <iostream>
using namespace std;

class packet{
	public:
		packet();
		int buildPacketHeader();
		int buildPacketQuestion();
	private:
		vector<vector<char> > octets;
};

	packet::packet(){
		cout << "creating packet\n";
		buildPacketHeader();
	}

	int packet::buildPacketHeader(){
		octets.push_back(vector<char>(8,0)); //it really doesn't matter the ID
		octets.push_back(vector<char>(8,1)); //but we do some stuff. so 1's!
		for(int i = 0; i< 10; i++){
			octets.push_back(vector<char>(8,0)); //all other values are 0
		}
		cout << "packet has been made\n";
		return 0;
	}

int main(){
	packet outgoing = packet();
	cout << "ending packet creation\n";
	return 0;
}
	
