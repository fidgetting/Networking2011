/*
 * threadpool.cpp
 *
 *  Created on: Mar 7, 2012
 *      Author: norton
 */

#include <threadpool.hpp>

/**
 * Constructor simply set the head and tail to NULL and sets the number of
 * threads. This does not start the threads in the thread pool. For that one
 * must call threadpool::start() after the constructor has finished.
 *
 * @param threads  the number of threads in the thread pool
 */
scaling::threadpool::threadpool(uint8_t threads) :
_lock(), _wait(), _threads(), _queue(), _running(true),
_nthreads(threads),_waiting(0), _working(0) { }

/**
 *
 */
void
scaling::threadpool::stop() {
  {
    std::unique_lock<std::mutex> _l(_lock);
    _running = false;
    _wait.notify_all();
  }

  for(std::shared_ptr<std::thread> curr : _threads) {
    curr->join();
  }
}

/**
 * TODO
 *
 * @return
 */
std::function<void(void)>
scaling::threadpool::_take() {
  std::unique_lock<std::mutex> _l(_lock);
  std::function<void(void)> retval;

  while(_queue.size() == 0) {
    if(!_running)
      return NULL;

    _waiting++;
    _working--;

    _wait.wait(_l);

    _working++;
    _waiting--;

    if(!_running)
      return NULL;
  }

  retval = _queue.front();
  _queue.pop_front();
  return retval;
}

/**
 * TODO
 *
 * @param id
 */
void
scaling::threadpool::_exec_thread(uint8_t id) {
  std::function<void(void)> curr;

  while(_running) {
    if((curr = _take()) != NULL) {
      curr();
      curr = NULL;
    }
  }
}


