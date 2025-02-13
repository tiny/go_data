/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       gostopwatch.h
  @brief      GOStopWatch class definition

  @author     rmcinnis@solidice.com
  @date       january 12, 1998
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#ifndef GOSTOPWATCH_H
#define GOSTOPWATCH_H

#include <gostring.h>
#include <pointintime.h>
#include <gotimer.h>

namespace go {

/*!
  @class GOStopWatch
   
  <b>Description:</b>
   
  <b>Notes:</b>
  
   
  @author     rmcinnis@solidice.com 
  @date       january 12, 1998
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
class GOStopWatch : public GOString
{
  protected :
    PointInTime    _start_ts ;
    bool           _isRunning ;
    int32_t           _diff ;
    GOString       _fmt ; // default value:  "%3d:%02d.%03d"
    HTIMER         _hTimer ;
    GOTimer       *_timer ;

    virtual int16_t  on_timer( const ArgList *args ) ;
    virtual void   render() ;
    virtual void   render_string( int32_t dt ) ; // override this virtual to change string output (dt == milliseconds)

  public    :
                   GOStopWatch( GOTimer *tymer ) ; // default constructor
                   GOStopWatch( const GOStopWatch &i ) ;
    virtual       ~GOStopWatch() ;

    // controls
    virtual void   reset() ;
    virtual void   start() ;
    virtual void   stop() ;

    // access methods
    GOString      &fmt() { return _fmt ; }
} ; // class GOStopWatch

} // namespace go 

#endif
