/*
 * Scheduler.cpp
 *
 *  Created on: May 10, 2012
 *      Author: norton
 */

/* local includes */
#include <Scheduler.hpp>

/* std library includes */
#include <sstream>
#include <string>

/* library includes */
#include <boost/program_options.hpp>
namespace po = boost::program_options;
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

/// used during the sig handlers to get at a scheduler
sched::Scheduler* sched::Scheduler::forSigs = NULL;

/**
 * Constructor for the Scheduler class.
 *
 * @param argc  number of arguments, passed directly from main
 * @param argv  the command line arguments, passed directly from main
 */
sched::Scheduler::Scheduler(int argc, char** argv) :
    _pid(getpid()), _port(23456), _verbose(0), _config_path(), _log_path(),
    _log() {
  /* command line and config file parsing */
  po::options_description option("Generic options");
  po::options_description config("Configuration");
  po::options_description hidden("Hidden options");

  po::options_description cmdl;
  po::options_description file;

  po::variables_map vm;

  /* cmd line and config options */
  std::string conf_str;
  std::string log_str;
  std::string addr_str;

  /* setup defaults */
  std::ostringstream ostr;
  EventQueue::Log() = &_log;
  ostr << _pid;
  _log.name() = "scheduler";
  _log.msg()  = ostr.str();

  forSigs = this;

  signal(SIGCHLD, handle_sig);
  signal(SIGALRM, handle_sig);
  signal(SIGTERM, handle_sig);
  signal(SIGQUIT, handle_sig);
  signal(SIGHUP,  handle_sig);

  /* declare options that can be found on the command line */
  option.add_options()
      /* command line options without arguments */
      ("help,h",      "Produce help message"                         )
      ("daemon,d",    "Run the scheduler as a daemon"                )
      ("kill,k",      "Forcibly kill any other schedulers"           )
      ("shutdown,s", "Gracefully shutdown any other schedulers"     )
      ("reset,R",     "Reset the job queue upon startup"             )
      ("test,t",      "Close the scheduler after the startup tests"  )
      /* command line options with arguments */
      ("config,c", po::value<std::string>(&conf_str),
          "Change the location of the system configuration file")
      ("log,L", po::value<std::string>(&log_str),
          "Change the location of the log file")
      ("port,P", po::value<uint16_t>(&_port),
          "What port is the scheduler interface listening on")
      ("verbose,v", po::value<uint32_t>(&_verbose),
          "Verbosity level for the scheduler");

  /* options that can be found in the configuration file */
  config.add_options()
      ("address", po::value<std::string>(&addr_str),
          "The address of the machine the scheduler is running on");

  hidden.add_options()
      ("logging.name", po::value<std::string>(&_log.name()),
          "The name of this processed as displayed in the log file")
      ("logging.message", po::value<std::string>(&_log.msg()),
          "The message that will be displayed in the log file")
      ("logging.indent", po::value<uint8_t>(&_log.indent_size()),
          "The amount to indent categories by in the log file");

  /* perform the parsing */
  cmdl.add(option).add(config);
  file.add(option).add(config).add(hidden);
  po::store(po::command_line_parser(argc, argv).options(cmdl).run(), vm);
  po::notify(vm);

  _config_path = conf_str;
  _log_path    = log_str;

  // TODO this needs to work
  //vm.clear();
  //po::store(po::command_line_parser(argc, argv).options(cmdl).run(), vm);
  //po::store(po::parse_config_file(conf_str.c_str(), file), vm);
  //po::notify(vm);

  if(vm.count("help"))   { cmdl.print(std::cout); exit(0); }
  if(vm.count("daemon"))   daemon(0, 0);
  if(vm.count("kill"))     kill(false);
  if(vm.count("shutdown")) kill(true);
  if(vm.count("reset"))    resetDatabase();
  if(vm.count("log"))      _log.open(log_str);


  event_signal(&Scheduler::logHeader, this);
  EventQueue::Signal(EventQueue::no_name, [] () { EventQueue::Terminate(); } );
  EventQueue::Enter([](){});
}

/**
 * @brief Puts the header into the log file
 *
 * This is the first event that gets called, so it truely signals that the
 * scheduler has started up.
 */
void sched::Scheduler::logHeader() {
  std::string path = _config_path.c_str();

  NOTICE(_log) << "*********************************************************\n";
  NOTICE(_log) << "***            FOSSology scheduler started            ***\n";
  NOTICE(_log) << "***        pid:    " << setw(27) << _pid <<  "        ***\n";
  NOTICE(_log) << "***        verb:" << setw(30) << _verbose << "        ***\n";
  NOTICE(_log) << "***        config:  " << setw(26) << path << "        ***\n";
  NOTICE(_log) << "*********************************************************\n";
  _log << sched::flush;
}

/**
 * Static function that can be passed to the operating system
 *
 * @param signo  the signal received by the scheduler
 */
void sched::Scheduler::handle_sig(int signo) {
  if(signo == SIGCHLD) {
    forSigs->handle_chld(signo);
  } else {
    forSigs->handle_prnt(signo);
  }
}

/**
 * Function that handles a SIGCHLD from the operating system. This is emitted
 * when an Agent dies. Pass the information off to the agent that died
 *
 * @param signo  will always be a SIGCHILD
 */
void sched::Scheduler::handle_chld(int signo) {
  pid_t  n;
  int status;

  while((n = waitpid(-1, &status, WNOHANG)) > 0)
  {
    SCHED_VERB(log()) << "SIGNALS: received sigchld for pid " << n
        << sched::endl;
    event_signal(&Agent::death, _agents[n], status);
  }
}

/**
 * Function that handles all signal received by the scheduler that aren't
 * SIGCHLDs. This basically decides what action to take when a signal is
 * received.
 *
 * @param signo
 */
void sched::Scheduler::handle_prnt(int signo) {
  switch(signo) {
    case SIGALRM: break;
    case SIGTERM: break;
    case SIGQUIT: break;
    case SIGINT:  break;
    case SIGHUP:  break;

    default:
      ERROR(log()) << " Received unknown signal: " << signo << sched::endl;
      break;
  }
}

/* TODO remove me */
void sched::Scheduler::kill(bool gracefull) {
  _log << "CALL: Scheduler::kill" << sched::endl;
}

bool sched::Scheduler::resetDatabase() {
  _log << "CALL: Scheduler::resetDatabase" << sched::endl;
  return true;
}

void sched::Agent::death(int status) {

}
















































