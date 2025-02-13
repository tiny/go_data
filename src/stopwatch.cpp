/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       stopwatch.cpp
  @brief      StopWatch class implementation

  @author     rmcinnis@solidice.com
  @date       january 12, 2002
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#if defined(_WINDOWS) && !defined(_AFXDLL)
#  include <windows.h>
#elif defined(UNIX)
#  include <sys/time.h>
#endif
#include <stopwatch.h>

namespace go {

StopWatch :: StopWatch()
{
  _p1 = _p2 = 0 ;

  // initializing
#if defined(_WINDOWS) || defined(WIN32)
  _diff = _resolution = 0 ;

  LARGE_INTEGER  units ;

  if (QueryPerformanceFrequency(&units)) // non-zero return == non-failure
  {
    _resolution = units.QuadPart ;
  }
#elif defined(UNIX)
#endif
} // StopWatch [constructor]

int64_t StopWatch :: diff_usec() 
{
#if defined(_WINDOWS) || defined(WIN32)
  if (_resolution == 0)
    return 0 ;
  return (int64_t)(1000000.0*(double)_diff)/(double)_resolution ;
#elif defined(UNIX)
  int32_t  sec, usec ;
  sec = (_p2 >> 32) - (_p1 >> 32) ;
  usec = (_p2 & 0xffffffff) - (_p1 & 0xffffffff) ;
  return sec * 1000000L + usec ;
#endif
} // StopWatch :: diff_usec

void StopWatch :: start()
{
#if defined(_WINDOWS) || defined(WIN32)
  LARGE_INTEGER  p ;

  _p1 = _p2 = _diff = 0 ;
  if (QueryPerformanceCounter(&p))
  {
    _p1 = p.QuadPart ;
  }
#elif defined(UNIX)
  struct timeval  tm ;
  struct timezone tz ;
  gettimeofday( &tm, &tz ) ;
  _p1 = (((int64_t)tm.tv_sec) << 32) + tm.tv_usec ; 
#endif
} // StopWatch :: start

void StopWatch :: stop() 
{
#if defined(_WINDOWS) || defined(WIN32)
  LARGE_INTEGER  p ;

  if (QueryPerformanceCounter(&p))
  {
    _p2   = p.QuadPart ;
    _diff = _p2 - _p1 ;
  }
#elif defined(UNIX)
  struct timeval  tm ;
  struct timezone tz ;
  gettimeofday( &tm, &tz ) ;
  _p2 = (((int64_t)tm.tv_sec) << 32) + tm.tv_usec ; 
#endif
} // StopWatch :: stop

} // namespace go 
