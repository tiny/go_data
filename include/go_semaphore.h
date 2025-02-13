/*
  go_semaphore.h
  semaphore class definition

  © 2024 Robert McInnis <r_mcinnis@solidice.com>
*/
#pragma once 
#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <mutex>
#include <semaphore>
#include <condition_variable>
#if __cplusplus >= 202002L
   // Code specific to C++20 or later
#  define SEM_T  std::condition_variable
#elif __cplusplus >= 201103L
   // Code specific to C++11 or later
//#  define SEM_T  std::condition_variable
#else
    #error "This code requires at least C++11"
#endif

namespace go {

  class semaphore
  {
  protected:
    std::mutex _mtx;
    std::condition_variable _condition;
    uint32_t _count;

  public:
    semaphore(uint32_t c = 0)
    {
      _count = c;
    }
    uint32_t count() { return _count; }
    void clear() 
    {
      std::lock_guard<decltype(_mtx)> lock(_mtx);
      _count = 0 ;
    }
    void post()
    {
      std::lock_guard<decltype(_mtx)> lock(_mtx);
      ++_count;
      _condition.notify_one();
    }

    int16_t wait()
    {
      std::unique_lock<decltype(_mtx)> lock(_mtx);
      while (!_count)
      {
        _condition.wait(lock);
      }
      --_count;
      return 0;
    }

    inline int16_t wait(int32_t ms_timeout)
    {
      std::unique_lock<std::mutex> lock(_mtx);
      auto now = std::chrono::system_clock::now();
      int16_t  rc = 0;
      if (_condition.wait_until(lock, now + std::chrono::microseconds(ms_timeout * 1000), [this]() {return (_count > 0); }))
      {
        _count--;
        rc = 0;
      }
      else
      {
        rc = -1;
      }
      return rc;
    }
  }; // class semaphore

} // namespace go 

#endif
