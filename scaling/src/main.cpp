/*
 * main.cpp
 *
 *  Created on: Apr 2, 2012
 *      Author: norton
 */

#include <Socket.hpp>

#include <iostream>
#include <map>

#include <opencv2/opencv.hpp>

/* ************************************************************************** */
/* ***  video data  ********************************************************* */
/* ************************************************************************** */

struct video_data : public soc::Message {
    video_data(int num = 0, std::string fname = "") { }
    virtual ~video_data() { }

    virtual void Serialize  (const soc::Stream& dst) const;
    virtual void Deserialize(const soc::Stream& src);

    int         num;
    std::string name;
};

void video_data::Serialize(const soc::Stream& dst) const {
  dst << num << name;
}

void video_data::Deserialize(const soc::Stream& src) {
  src >> num >> name;
}

/* ************************************************************************** */
/* ***  image data  ********************************************************* */
/* ************************************************************************** */

struct image_data : public soc::Message {
  image_data(cv::Mat img = cv::Mat()) : image(img) { }
  virtual ~image_data() { }

  virtual void Serialize  (const soc::Stream& dst) const;
  virtual void Deserialize(const soc::Stream& src);

  cv::Mat image;
};

void image_data::Serialize(const soc::Stream& dst) const {
  uint64_t size = image.dataend - image.data;

  dst << image.rows << image.cols << image.type() << size;
  dst.Write(image.data, 0, size);
}

void image_data::Deserialize(const soc::Stream& src) {
  uint64_t size;
  int rows, cols, type;

  src >> rows >> cols >> type >> size;

  image = cv::Mat::zeros(rows, cols, type);

  src.Read(image.data, 0, size);
}

/* ************************************************************************** */
/* ***  main functions  ***************************************************** */
/* ************************************************************************** */

void image_server(std::string name) {
  cv::VideoCapture cap(name);
  cv::Mat img;

  soc::Socket server(23456);
  soc::Socket client = server.Accept();
  soc::Stream stream(client);

  uint32_t nframes = cap.get(CV_CAP_PROP_FRAME_COUNT);
  std::shared_ptr<image_data> ret;

  video_data vid(300, name);
  stream << vid;

  for(int i = 0; i < vid.num; i++) {
    /* send the image accross the network */
    cap >> img;
    image_data data(img);
    stream << data;

    /* get the altered image */
    ret = soc::Message::Get<image_data>(stream);
    cv::imshow("win", std::dynamic_pointer_cast<image_data>(ret)->image);
    cv::waitKey(30);
  }

  cv::waitKey(-1);
}

void image_client(std::string addr) {
  cv::Mat img;

  soc::Socket conn(addr, 23456);
  soc::Stream str(conn);
  std::shared_ptr<video_data> vid;
  std::shared_ptr<image_data> ret;

  vid = soc::Message::Get<video_data>(str);

  for(int i = 0; i < vid->num; i++) {
    ret = soc::Message::Get<image_data>(str);

    cv::cvtColor(ret->image, img, CV_RGB2HSV);

    image_data send(img);
    str << send;
  }
}

int main(int argc, char** argv) {

  soc::Message::Register([](){ return new video_data(); });
  soc::Message::Register([](){ return new image_data(); });

  if(argc == 2) {
    image_server(argv[1]);
  } else {
    image_client(argv[1]);
  }
}

