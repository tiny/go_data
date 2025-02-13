/*
  now.h
  now time functional implementation

  © 2024 Robert McInnis <r_mcinnis@solidice.com>
*/
#include <stdlib.h>
#include <cstring>
#include <time.h>
#include "now.h"
#include <chrono>

#if defined(LINUX)
#  include <sys/time.h>
#endif

namespace go {

uint64_t  __timeGetTime = 0 ;  // microseconds

#ifdef WIN32
#include <windows.h>
uint64_t getSystemUptime() {
//  ULONGLONG mseconds = GetTickCount64();
//  ULONGLONG useconds = mseconds * 1000;
//  return std::chrono::microseconds(useconds);
  return GetTickCount64() * 1000;
} // :: getSystemUptime
#endif

uint64_t now() 
{
#ifdef LINUX
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000000 + tv.tv_usec;
#else
  return getSystemUptime();
#endif
} // :: now

uint32_t now_timet() 
{
#ifdef LINUX
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec ;
#else
  return now() / 1000;
#endif
} // :: now_timet

#ifdef LINUX
uint32_t timeGetTime()
{
  uint64_t t1 = now() ;
  if (__timeGetTime == 0)  __timeGetTime = t1 ;
  return (t1 - __timeGetTime) / 1000 ;
} // :: timeGetTime
#endif

uint64_t timeGetTime64()
{
  uint64_t t1 = now() ;
  if (__timeGetTime == 0)  __timeGetTime = t1 ;
  return (t1 - __timeGetTime) ;
} // :: timeGetTime64

/* some tests:

  printf( "%s \n", time2str_sec ( 1722125299       ).c_str() ) ; 
  printf( "%s \n", time2str_msec( 1722125299656    ).c_str() ) ; 
  printf( "%s \n", time2str_usec( 1722125299656495 ).c_str() ) ; 
  printf( "%s \n", time2str_sec ( 16*365*24*3600 + 15*24*3600 + 8*3600 + 25*60 ).c_str() ) ; 
  printf( "%s \n", time2str_sec ( 15*24*3600 + 8*3600 + 25*60 ).c_str() ) ; 
  printf( "%s \n", time2str_msec( (8*3600 + 25*60 + 10) * 1000 + 155 ).c_str() ) ;
  printf( "%s \n", time2datestr( "%d-%b-%0y %0H:%0M:%0S", 1722125299 ).c_str() ) ;  

  results:
  54y 222d 00:08:19 
  54y 222d 00:08:19.656 
  54y 222d 00:08:19.656495 
  16y 015d 08:25:00 
  015d 08:25:00 
  08:25:10.155 
  28-Jul-24 00:08:19
*/

std::string time2str_sec( uint32_t t ) // t in seconds
{
  uint64_t  t1 = t ;
  uint16_t  yrs ;
  uint16_t  days ;
  uint16_t  hrs ;
  uint16_t  mins ;
  uint16_t  sec ;

  sec = t % 60 ;
  t   /= 60 ;     //  -> minutes
  mins = t % 60 ; 
  t   /= 60 ;     // -> hours
  hrs  = t % 24 ; 
  t   /= 24 ;     // -> days
  days = t % 365 ;
  t   /= 365 ;     // -> years
  yrs  = t ;

  char b1[64] ;
  ::memset( b1, 0x00, 64 ) ;
  if (yrs) snprintf( b1, 64, "%dy %03dd ", yrs, days ) ;
  else if (days) snprintf( b1, 64, "%03dd ", days ) ;

  char b2[255] ;
  snprintf( b2, 255, "%s%02d:%02d:%02d", b1, hrs, mins, sec ) ;

  return b2 ;
} // :: time2str_sec

std::string time2str_msec( uint64_t t ) // t in milliseconds
{
  uint32_t t1 = (uint32_t)(t / 1000) ;
  std::string str = time2str_sec( t1 );
  return str + std::string(".") + std::to_string(t % 1000) ;
} // :: time2str_usec

std::string time2str_usec( uint64_t t ) // t in microseconds
{
  return time2str_sec( (uint32_t)(t / 1000000) ) + "." + std::to_string(t % 1000000) ;
} // :: time2str_usec

std::string time2datestr( const char *fmt, uint32_t t ) 
{
  time_t raw_time = t ;
  struct tm *timeinfo;
  timeinfo = localtime( &raw_time );
  
  char buf[256] ;
  strftime( buf, 255, fmt, timeinfo ) ;
  return buf ;
} // :: time2datestr

std::string time2datestr( uint64_t t ) 
{
  uint32_t  n = t / 1000000 ;
  return time2datestr( "%d%h%Y %H:%M:%S.", n ) + std::to_string( t % 1000000 );
} // :: time2datestr

} // namespace go 
