/*
 * Scheduler.hpp
 *
 *  Created on: May 10, 2012
 *      Author: norton
 */

#pragma once

/* local includes */
#include <Agent.hpp>
#include <Event.hpp>
#include <Logging.hpp>

/* std library includes */
#include <map>

/* library includes */
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

namespace sched {

  class Scheduler {
    public:

      Scheduler(int argc, char** argv);


      inline static bool verb_sched() { return forSigs->_verbose & 0x20; }
      inline static bool verb_event() { return forSigs->_verbose & 0x40; }

      inline sched::Log& log() { return _log; }

      void kill(bool gracefull);
      bool resetDatabase();

      inline Agent& agent(pid_t pid) { return _agents[pid]; }

    protected:

      static Scheduler* forSigs;

      static void handle_sig(int signo);
      void handle_chld(int signo);
      void handle_prnt(int signo);

      void logHeader();

      std::map<pid_t, Agent> _agents;

      /// the PID of the scheduler process
      pid_t _pid;
      /// port that the scheduler interface will listen on
      uint16_t _port;
      /// verbose setting for the scheduler
      uint32_t _verbose;

      /// the location of the configuration data
      fs::path _config_path;
      /// the location of the logging file
      fs::path _log_path;
      /// the log file
      sched::Log _log;
  };


}



