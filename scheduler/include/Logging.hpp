/*
 * Logging.hpp
 *
 *  Created on: May 10, 2012
 *      Author: norton
 */

#pragma once

/* local includes */
#include <Event.hpp>

/* std library includes */
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdint.h>
#include <string.h>
#include <thread>

namespace sched {

  class Log;
  class _WidthManip;

#define oprcon \
    if(std::this_thread::get_id() != EventQueue::Thread()) { \
      _buff << val;                                          \
                                                             \
      if(_buff.str().size() > _max_buffer) {                 \
        flush();                                             \
      }                                                      \
    } else {                                                 \
      _ostr << val;                                          \
    }                                                        \
    return *this


#define ERROR(log)   log << "ERROR: ["   << __FILE__ << "." << __LINE__ << "] "
#define WARNING(log) log << "WARNING: [" << __FILE__ << "." << __LINE__ << "] "
#define NOTICE(log)  log << "NOTICE: ["  << __FILE__ << "." << __LINE__ << "] "

#define SCHED_VERB(log) if(verb_sched()) log

  class Log {
    public:

      Log();
      Log(std::string fname);
      ~Log() { close(); }

      void endl();
      void flush();
      void catb();
      void cate();

      void open(std::string fname);
      inline void close()
      { _ostr << std::endl; _ostr.close(); _buff.str(""); }

      inline std::streamsize width() const
      { return _ostr.width(); }
      inline std::streamsize width(std::streamsize wide)
      { return _ostr.width(wide); }

      /* getters and setters */
      inline std::string  name()        const { return _name;        }
      inline std::string& name()              { return _name;        }
      inline std::string  msg()         const { return _msg;         }
      inline std::string& msg()               { return _msg;         }
      inline uint32_t     max_buffer()  const { return _max_buffer;  }
      inline uint32_t&    max_buffer()        { return _max_buffer;  }
      inline uint8_t      indent_size() const { return _indent_size; }
      inline uint8_t&     indent_size()       { return _indent_size; }

      /* just getters */
      inline bool    is_open() const { return _ostr.is_open(); }
      inline uint8_t indent()  const { return _indent;         }

      inline Log& operator<<(bool val)           { oprcon; }
      inline Log& operator<<(short val)          { oprcon; }
      inline Log& operator<<(unsigned short val) { oprcon; }
      inline Log& operator<<(int val)            { oprcon; }
      inline Log& operator<<(unsigned int val)   { oprcon; }
      inline Log& operator<<(long val)           { oprcon; }
      inline Log& operator<<(unsigned long val)  { oprcon; }
      inline Log& operator<<(float val)          { oprcon; }
      inline Log& operator<<(double val)         { oprcon; }
      inline Log& operator<<(long double val)    { oprcon; }

      inline Log& operator<<(const char* val)
      { return this->operator<<(std::string(val)); }
      inline Log& operator<<(void(*_f)(Log&))
      { _f(*this); return *this; }

      Log& operator<<(std::string val);

    protected:

      void line_start();

      /// the name of the process that is being logged for
      std::string _name;
      /// message that will get on each line
      std::string _msg;
      /// the maximum size of the asynchronous buffer
      uint32_t _max_buffer;
      /// how many spaces to indent each time
      uint8_t _indent_size;
      /// the number of indents we are at
      uint8_t _indent;

      /// the actual io stream used for the log
      std::ofstream      _ostr;
      /// buffer used for asynchronous logging operations
      std::ostringstream _buff;
  };

  inline void endl  (Log& ostr) { ostr.endl();  }
  inline void flush (Log& ostr) { ostr.flush(); }
  inline void catb  (Log& ostr) { ostr.catb();  }
  inline void cate  (Log& ostr) { ostr.cate();  }

  class _WidthManip {
    public:

      _WidthManip(uint32_t _width) : _width(_width) { }
      virtual ~_WidthManip() { }

      virtual void exec(Log& l) { l.width(_width); }

      uint32_t _width;
  };

  inline _WidthManip setw(uint32_t width)
  { return _WidthManip(width); }
  inline Log& operator<<(Log& ostr, _WidthManip width)
  { ostr.width(width._width); return ostr; }

#undef oprcon
}


