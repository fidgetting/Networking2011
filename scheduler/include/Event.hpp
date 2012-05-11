/*
 * Event.hpp
 *
 *  Created on: May 10, 2012
 *      Author: norton
 */

#pragma once

/* std library includes */
#include <condition_variable>
#include <deque>
#include <functional>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

/// Use this macro to scheduler new events instead of EventQueue::Signal
#define event_signal(Functor, ...) \
    sched::EventQueue::Signal(#Functor, Functor, ##__VA_ARGS__)

namespace sched {

  /// forward declaration
  class Log;

  /**
   * @brief an EventQueue or EventLoop used for a main processing loop
   *
   * This is used when multiple threads need to be able to pass work off to
   * a main thread instead of executing it themselves. As a result, this is
   * a single class and all interactions with it are performed using static
   * function that call upon the singleton instance.
   */
  class EventQueue {
    public:

      template<typename _Functor, typename... _Args>
      static void Signal(std::string name, _Functor&& func, _Args&&... args);

      template<typename _Functor, typename... _Args>
      static void Enter(_Functor&& func, _Args&&... args);

      static void Terminate();

      inline static std::thread::id Thread() { return _singleton._thread; }
      inline static sched::Log*&    Log()    { return _singleton._ostr;   }

      static std::string no_name;

    protected:

      EventQueue();

      /* cannot create another event queue instance */
      EventQueue(EventQueue& ev)                         = delete;
      EventQueue(EventQueue&& ev)                        = delete;
      const EventQueue& operator=(const EventQueue& asn) = delete;

      template<typename _Functor>
      void push(std::string name, _Functor&& func);

      void enter(std::function<void(void)> func);

      /// The event queue singleton
      static EventQueue _singleton;

      /// whether a thread is already working in the queue
      std::thread::id         _thread;
      /// mutex to control queue access
      std::mutex              _lock;
      /// condition variable to wake the working thread
      std::condition_variable _wait;

      /// the actual queue
      std::deque<std::pair<std::string, std::function<void(void)>>> _queue;
      /// the log file currently being used
      sched::Log* _ostr;

      /// used to terminate the event loop
      struct FlowController { };
  };

  /**
   * @brief pushes a new event into the EventQueue
   *
   * @param name  the name of the function
   * @param func  the function that will be called
   * @param args  any arguments to the function
   */
  template<typename _Functor, typename... _Args>
  void EventQueue::Signal(std::string name, _Functor&& func, _Args&&... args) {
    _singleton.push(name, std::bind(
        std::forward<_Functor>(func),
        std::forward<_Args>(args)...));
  }

  /**
   * @brief entry point for a thread to work on the event queue
   *
   * @param func  the function that will be used to update the event queue
   * @param args  the arguments to the function
   */
  template<typename _Functor, typename... _Args>
  void EventQueue::Enter(_Functor&& func, _Args&&... args) {
    _singleton.enter(std::bind(
        std::forward<_Functor>(func),
        std::forward<_Args>(args)...));
  }

  /**
   * @brief Puts a new event into the queue.
   *
   * @param name  the name of the event that is being called
   * @param func  the functor that will be called
   */
  template<typename _Functor>
  void EventQueue::push(std::string name, _Functor&& func) {
    std::unique_lock<std::mutex> _l(_lock);

    _queue.push_back(std::pair<std::string, std::function<void(void)>>(
        name, std::forward<_Functor>(func)));

    _wait.notify_all();
  }
}
