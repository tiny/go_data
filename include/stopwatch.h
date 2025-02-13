/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       stopwatch.h
  @brief      StopWatch class definition

  @author     rmcinnis@solidice.com
  @date       january 12, 2002
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#ifndef STOPWATCH_H
#define STOPWATCH_H

#include <enum.h>

namespace go {

/*!
  @class StopWatch
   
  <b>Description:</b>

  <b>Notes:</b>
   
  @author     rmcinnis@solidice.com 
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
class StopWatch
{
  private :
    int64_t     _p1 ;
    int64_t     _p2 ;
#if defined(_WINDOWS) || defined(WIN32)
    int64_t     _diff ;
    int64_t     _resolution ;
#endif

  public  :
              StopWatch() ;

    int64_t     diff_usec() ;
    void      start() ;
    void      stop() ;

    // access methods
    int64_t    &p1() { return _p1 ; }
    int64_t    &p2() { return _p2 ; }
} ; // class StopWatch

} // namespace go 

#endif
