/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       timedevent.cpp
  @brief      TimedEvent class implementation

  @author     rmcinnis@solidice.com
  @date       january 12, 1998
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#include "timedevent.h"
#ifdef WIN32
#  include <mmsystem.h>
#else
#  include "now.h"
#  include <unistd.h>
#endif

namespace go {

int32_t TimedEvent :: nActive = 0 ;
int32_t TimedEvent :: nUsed = 0 ;

TimedEvent :: TimedEvent( HTIMER id, Callback *cb, uint32_t delay_ms, uint32_t next_tm, bool oneshot ) 
{
  _id        = id ;
  _cb        = cb ;
  _delay     = max( (uint32_t)MIN_TIMER_DELAY, min( (uint32_t)MAX_TIMER_DELAY, delay_ms )) ; // valid range: 1ms...7days
  _tm        = (next_tm == 0) ? timeGetTime() + _delay : next_tm ;
  _oneshot   = oneshot ;
  _ok2del    = false ;
  
  nActive++ ;
  nUsed++ ;
} // TimedEvent [constructor]

TimedEvent :: TimedEvent( const TimedEvent &c )
{
  *this = c ;

  nActive++ ;
  nUsed++ ;
} // TimedEvent [constructor]

TimedEvent :: ~TimedEvent() 
{
  clear() ;

  nActive-- ;
} // TimedEvent [destructor]

void TimedEvent :: clear() 
{ 
//  if (_ok2del)
//    set_cb( nullptr ) ;

  _cb = nullptr ; 
  _tm = 0 ; 
  _delay = -1 ; 
} // TimedEvent :: clear

void TimedEvent :: set_cb( Callback *cb )
{
  delete _cb ;
  _cb = cb ;
} // TimedEvent :: set_cb

void TimedEvent :: start() 
{ 
  _tm = timeGetTime() + _delay ; 
} // TimedEvent :: start

void TimedEvent :: stop( bool ok2del ) 
{ 
  _tm = 0 ; 
  _ok2del = ok2del ;
} // TimedEvent :: stop

int16_t TimedEvent :: trigger( const ArgList &args, uint32_t now, int32_t threshold )
{
  if (_tm == 0)
    return -1 ;
  if ((int32_t)(_tm - now) > threshold)
    return -2 ;
  if (_cb != nullptr)
  {
    try 
    {
      _cb->invoke( args ) ;
    } 
    catch (...)
    {
      return -4 ; 
    }
  }
  _tm = (_oneshot || (_cb == nullptr)) ? 0 : now + _delay ; // does not account for invoke time
  return (_tm == 0) ? -3 : 0 ;
} // TimedEvent :: trigger

TimedEvent &TimedEvent :: operator= ( const TimedEvent &c )
{
  if (this != &c)
  {
    _id        = c._id ;
    _cb        = c._cb ;
    _delay     = c._delay ;
    _tm        = c._tm ;
    _oneshot   = c._oneshot ;
    _ok2del    = c._ok2del ;
  }
  return *this ;
} // TimedEvent :: operator=

bool TimedEvent :: operator< ( const TimedEvent &c ) const
{
  return ((_tm < c._tm) && (_tm != 0)) ;
} // TimedEvent :: operator<

} // namespace go 
