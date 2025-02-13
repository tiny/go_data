/*
  now.h
  now time functional prototype

  © 2024 Robert McInnis <r_mcinnis@solidice.com>
*/
#pragma once
#ifndef NOW_H
#define NOW_H

#include <stdint.h>
#include <string>

namespace go {

// returns the number of microseconds since jan 1, 1970
//
  uint64_t     now() ;
  uint32_t     now_timet() ;
#ifdef LINUX
  uint32_t     timeGetTime() ; // milliseconds since first call
#endif
  uint64_t     timeGetTime64() ; // microseconds since first call

  std::string  time2str_sec ( uint32_t t ) ; // yyyy-ddd-hh:mm:ss
  std::string  time2str_msec( uint64_t t ) ; // yyyy-ddd-hh:mm:ss.mmm
  std::string  time2str_usec( uint64_t t ) ; // yyyy-ddd-hh:mm:ss.mmmuuu

  std::string  time2datestr ( const char *fmt, uint32_t t ) ; 
  std::string  time2datestr ( uint64_t t ) ; // to the microsecond

} // namespace go 

#endif
