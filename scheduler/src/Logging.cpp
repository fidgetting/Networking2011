/*
 * Logging.cpp
 *
 *  Created on: May 10, 2012
 *      Author: norton
 */

/* local includes */
#include <Logging.hpp>

/* std library includes */
#include <ctime>
#include <fstream>

/**
 * @brief Basic constructor
 */
sched::Log::Log() : _name(), _msg(), _max_buffer(1024), _indent_size(2),
    _indent(0), _ostr(), _buff() { }

/**
 * @brief constructor that opens the file name provided
 *
 * @param fname  the name of the log file
 */
sched::Log::Log(std::string fname) : _name(), _msg(), _max_buffer(1024),
    _indent_size(2), _indent(0), _ostr(), _buff() {
  open(fname);
  line_start();
}

/**
 * @brief inserts a newline into the log file
 *
 * This makes sure that all bookkeeping and concurrency considerations are
 * taken care of when writing the log file. This function for sched::endl
 * should always be used instead of a new line character to keep the log
 * file formating correct
 */
void sched::Log::endl() {
  if(std::this_thread::get_id() != EventQueue::Thread()) {
    /* queue up an event */
    EventQueue::Signal(
        EventQueue::no_name,
        [&] (std::string& str) { _ostr << str << std::flush; endl(); },
        _buff.str());

    /* clear the buffer */
    _buff.str("");
  } else {

    /* create the time string */
    _ostr << '\n';
    line_start();

    _ostr << std::flush;
  }
}

/**
 * @brief Flushes any buffer associated with the Log file
 *
 * If this is called from a thread that isn't the thread executing the
 * EventQueue, this will create a logging event instead of doing a normal
 * flush.
 */
void sched::Log::flush() {
  if(std::this_thread::get_id() != EventQueue::Thread()) {
    /* queue up the event */
    EventQueue::Signal(
        EventQueue::no_name,
        [&] (std::string& str) { *this << str; _ostr << std::flush; },
        _buff.str());

    /* clear the buffer */
    _buff.str("");

  } else {
    _ostr << std::flush;
  }
}

/**
 * @brief begins a new category in the log file
 */
void sched::Log::catb() {
  /* avoiding code duplication */
  const auto actions = [&] () {
    _ostr << '\n';
    line_start();
    _indent++;
    _ostr << "{\n";
    line_start();
  };

  if(std::this_thread::get_id() != EventQueue::Thread()) {
    flush();
    EventQueue::Signal(EventQueue::no_name, actions);
  } else {
    actions();
  }
}

/**
 * @brief ends the current category in the log file
 */
void sched::Log::cate() {
  /* avoiding code duplication */
  const auto actions = [&] () {
    if(_indent > 0) {
      _ostr << '\n';
      _indent--;
      line_start();
      _ostr << "}";
    }
  };

  if(std::this_thread::get_id() != EventQueue::Thread()) {
    flush();
    EventQueue::Signal(EventQueue::no_name, actions);
  } else {
    actions();
  }
}

/**
 * @brief opens a new ofstream that will be used for the logging
 *
 * Note: if "stdout" or "stderr" is passed to this function, std::cout or
 *       std::cerr will be used instead of the normal ofstream.
 *
 * @param fname the name of the file to open
 */
void sched::Log::open(std::string fname) {
  if(fname == "stdout") {
    _ostr.open("/dev/tty");
  } else if(fname == "stderr") {
    _ostr.open("/dev/tty");
  } else {
    _ostr.open(fname);
  }

  _buff.str("");
  line_start();
}

/**
 * @brief << operator for strings
 *
 * This needs to be special because strings can include '\n' characters.
 * Everywhere that there is a '\n' the string will be split. The standard
 * prepend for each line is inserted where the string is split. This means
 * that categories will still correctly lineup on the string.
 *
 * @param val
 * @return
 */
sched::Log& sched::Log::operator<<(std::string val) {
  if(std::this_thread::get_id() != EventQueue::Thread()) {
    _buff << val;

    if(_buff.str().size() > _max_buffer) {
      flush();
    }
  } else {
    size_t loc;

    while((loc = val.find('\n')) != std::string::npos) {
      std::string section = val.substr(0, loc);

      _ostr << section << '\n';
      line_start();
      val = val.substr(loc + 1);
    }

    _ostr << val;
  }

  return *this;
}

/**
 * @brief Print stuff that prepends every line in the log file
 */
void sched::Log::line_start() {
  time_t t = time(NULL);
  char* tmp, * curr;
  char time_buf[64];

  strftime(time_buf, sizeof(time_buf), "%F %T", localtime(&t));
  _ostr << time_buf << " " << _name << " [" << _msg << "] :: ";

  for(int i = 0; i < _indent; i++) {
    for(int j = 0; j < _indent_size; j++) {
      _ostr << ' ';
    }
  }
}
