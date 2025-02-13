/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       timedevent.h
  @brief      TimedEvent class definition

  @author     rmcinnis@solidice.com
  @date       january 12, 1998
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#ifndef TIMEDEVENT_H
#define TIMEDEVENT_H

#include "msgcb.h"
#include <set>

#define  HTIMER               int32_t
#define  MAX_TIMER_DELAY      (7*24*60*60*1000L) /* 7days */
#define  MIN_TIMER_DELAY      (2L)               /* 2ms - anything less is really just a busy loop*/
#define  AVG_TIMER_WAIT       (100L)             /* 100ms for empty timer wakeup */

namespace go {

class TimedEvent
{
  protected :
    HTIMER          _id ;
    Callback       *_cb ;
    int32_t            _delay ; // number of milliseconds between events
    bool            _oneshot ;
    uint32_t           _tm ;    // time of next event trigger 
    bool            _ok2del ; // delete callback on close
    
  public    :
                    TimedEvent( HTIMER id = 0, 
                                Callback *cb = nullptr, 
                                uint32_t delay_ms = 0, 
                                uint32_t next_tm = 0, 
                                bool oneshot = false ) ; 
                    TimedEvent( const TimedEvent &c ) ;
                   ~TimedEvent() ;

    void            clear() ;
    void            set_cb( Callback *cb ) ;
    void            start() ;
    void            stop( bool ok2del = false ) ;
    int16_t           trigger( const ArgList &args, uint32_t now, int32_t threshold = 1 ) ;
    TimedEvent     &operator= ( const TimedEvent &c ) ;
    bool            operator< ( const TimedEvent &c ) const ;

    // convienence methods
    HTIMER          id() const { return _id ; }
    HTIMER         &id() { return _id ; }
    Callback       *cb() const { return _cb ; }
    uint32_t           delay() const { return _delay ; }
    bool            oneshot() const { return _oneshot ; }
    uint32_t           next_tm() const { return _tm ; }

    // counters
    static int32_t     nActive ;
    static int32_t     nUsed ;
} ; // class TimedEvent

using namespace std; 

typedef multiset<TimedEvent>            TimedEventSet ;
typedef multiset<TimedEvent>::iterator  TimedEventSet_iter ;

} // namespace go 

#endif
