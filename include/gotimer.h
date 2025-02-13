/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       gotimer.h
  @brief      GOTimer class definition

  @author     rmcinnis@solidice.com
  @date       january 12, 1998
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#ifndef GOTimer_H
#define GOTimer_H

#include "timedevent.h"
#include "threads.h"
#include "govector.h"

namespace go {

// threaded GOTimer.  should be used for multiple GOTimer events
//
class GOTimer 
{
  protected :
    static HTIMER   _event_ticker ; // used for GOTimer ids
    MutexGate       _gate ;
    TimedEventSet   _events ;
    bool            _done ;
    int32_t            _delay ;
    Thread          _timer_t ;
    int32_t            _drift ;
    GOVector<HTIMER> _dead_timers ;
    ArgList         _args ; // args: uint32_t now, HTIMER id, bool *req_stop

    int32_t            action( uint32_t now ) ;
    int16_t           remove_the_dead() ;
    void            timer_proc() ;

  public    :
                    GOTimer() ;
                   ~GOTimer() ;

    void            clear() ;
    int32_t            nWatchers() { return _events.size() ; }
    void            report() ;
    size_t          size() const ;
    void            start() ;
    void            stop() ;
                   
    HTIMER          notify( uint32_t delay_ms, Callback *cb, bool oneshot = false ) ; // returns: >0 == timer-id, <0 == error
    int16_t           unnotify( HTIMER id, bool ok2del = false ) ;
    int16_t           unnotify( void *dest ) ; // will remove all timers to this receiver

    static int32_t     nTripped ; // number of times action() has been called
    static int32_t     nUsed ;
    static int32_t     nActive ;
} ; // class GOTimer

} // namespace go 

#endif

