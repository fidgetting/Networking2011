/*
 * Event.cpp
 *
 *  Created on: May 10, 2012
 *      Author: norton
 */

/* local includes */
#include <Event.hpp>
#include <Logging.hpp>
#include <Scheduler.hpp>

/// the event doesn't have a name
std::string sched::EventQueue::no_name("no_name");
/// the EventQueue instance
sched::EventQueue sched::EventQueue::_singleton;

/**
 * @brief Constructor for EventQueue
 */
sched::EventQueue::EventQueue() :
    _thread(), _lock(), _wait(), _queue(), _ostr() { }

/**
 * @brief Breaks the calling thread out of the event queue.
 */
void sched::EventQueue::Terminate() {
  if(std::this_thread::get_id() != _singleton._thread) {
    // TODO throw exception
  }
  throw FlowController();
}

/**
 * @brief the actual event loop
 *
 * This function will loop until one of the calling functions throws an
 * EventQueue::FlowController by making a call to EventLoop::Terminate().
 * This will simply take an item from the queue, execute it and then call
 * the supplied functor.
 *
 * @param func  update functor called after every event
 */
void sched::EventQueue::enter(std::function<void(void)> func) {
  std::string               name;
  std::function<void(void)> event;
  bool logging;

  try {
    /* lock queue */ {
      std::unique_lock<std::mutex> _l(_lock);
      if(_thread != std::thread::id()) {
        // TODO throw exception
      }

      _thread = std::this_thread::get_id();
    }

    /* the actual event loop */
    while(true) {
      /* lock queue */ {
        std::unique_lock<std::mutex> _l(_lock);

        if(_queue.size() == 0) {
          _wait.wait(_l);
        }

        name  = _queue.front().first;
        event = _queue.front().second;
        _queue.pop_front();
      }

      if(name != no_name && Scheduler::verb_event()) {
        *_ostr << "EVENT_START[ " << name << " ] " << sched::catb;
        logging = true;
      }
      event();
      if(logging) {
        *_ostr << sched::cate;
        logging = false;
      }

      func();
    }

  } catch(EventQueue::FlowController& f) {
    if(logging) {
      *_ostr << sched::cate;
      logging = false;
    }
  }

  /* lock queue */ {
    std::unique_lock<std::mutex> _l(_lock);
    _thread = std::thread::id();
  }
}
