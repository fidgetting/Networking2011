/*
 * main.cpp
 *
 *  Created on: Apr 2, 2012
 *      Author: norton
 */

#include <Socket.hpp>

#include <iostream>
#include <map>

class data : public soc::Message {
  public:

    data() { }
    virtual ~data() { }

    virtual void Serialize  (const soc::Stream& dst) const;
    virtual void Deserialize(const soc::Stream& src);

    std::map<int, std::string> the_map;
};

void data::Serialize(const soc::Stream& dst) const {
  dst << the_map;
}

void data::Deserialize(const soc::Stream& src) {
  src >> the_map;
}

int main(int argc, char** argv) {
  soc::Socket ser;
  soc::Socket cli;

  soc::Stream str;

  soc::Message::Register([](){ return new data(); });

  if(argc == 2) {
    ser = soc::Socket(atoi(argv[1]));
    cli = ser.Accept();
    str = soc::Stream(cli);

    data msg;
    msg.the_map[0] = "hello";
    msg.the_map[1] = "my";
    msg.the_map[2] = "name";
    msg.the_map[3] = "is";
    msg.the_map[4] = "amazing";

    str << msg;

  } else {
    cli = soc::Socket(argv[1], atoi(argv[2]));
    str = soc::Stream(cli);

    auto other = soc::Message::Get(str);

    for(auto curr : std::dynamic_pointer_cast<data>(other)->the_map) {
        std::cout << curr.second << " ";
      }
      std::cout << std::endl;
  }
}

