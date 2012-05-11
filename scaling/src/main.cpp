/*
 * main.cpp
 *
 *  Created on: Apr 2, 2012
 *      Author: norton
 */

#include <Socket.hpp>

#include <iostream>
#include <string>
#include <unordered_map>

#include <opencv2/opencv.hpp>

class data : public soc::Message {
  public:

    data() : Message() { }
    virtual ~data() { }

    virtual void   Serialize(const soc::Stream& dst) const;
    virtual void Deserialize(const soc::Stream& src);

    std::unordered_map<int, std::string> map;
};

void data::Serialize(const soc::Stream& dst) const {
  uint32_t size = map.size();

  dst << size;
  for(auto& curr : map) {
    dst << curr;
  }
}

void data::Deserialize(const soc::Stream& src) {
  std::pair<int, std::string> curr;
  uint32_t size;

  src >> size;
  for(int i = 0; i < size; i++) {
    src >> curr;
    map[curr.first] = curr.second;
  }
}

int main(int argc, char** argv) {
  soc::Message::Register( [](){ return new data(); } );
  soc::Socket serv(23456);
  soc::Socket conn = serv.Accept();
  soc::Stream str(conn);

  data d;

  d.map[0] = "Yay";
  d.map[1] = "it";
  d.map[2] = "worked";


}

