/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       gotimer.h
  @brief      GOTimer class implementation

  @author     rmcinnis@solidice.com
  @date       january 12, 1998
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#include "gotimer.h"
#ifdef WIN32
#  include <mmsystem.h>
#  include "si_sleep.h"
#else
#  include "now.h"
#  include <unistd.h>
#  include <algorithm>
#endif

namespace go {

HTIMER GOTimer :: _event_ticker = 100 ; // i don't like starting at 0
int32_t   GOTimer :: nTripped = 0 ;
int32_t   GOTimer :: nUsed = 0 ;
int32_t   GOTimer :: nActive = 0 ;

int32_t GOTimer :: action( uint32_t now ) 
{
  nTripped++ ;

  if (_events.size() == 0)
    return AVG_TIMER_WAIT ;

  TimedEventSet_iter  it ;
  MutexScope          sc( _gate ) ;
  TimedEvent          evt ;
  bool                req_stop = false ;
  static int32_t         nexceptions = 0 ;

  // determine drift; sleep timers are not exact... use a drift amount to figure out how to offset
  // note: the divide by 2 results in much more accurate timing
  //
  if (abs((int32_t)now - (int32_t)(*_events.begin()).next_tm()) < 10)
  {
    // drift in useconds
    _drift = (1000 * ((int32_t)(*_events.begin()).next_tm() - (int32_t)now)) / 2 ;
    _drift -= 400 ; // usec
  }

  while ((_events.size() > 0) && !_done)
  {
    if (remove_the_dead() != 0) // garbage collect dead timers
      continue ;

    evt = *_events.begin() ;
    now = timeGetTime() ;
    if (((int32_t)(evt.next_tm() - now) > MIN_TIMER_DELAY) && (evt.next_tm() > 0))
    {
      return max( MIN_TIMER_DELAY, min( (long)(evt.next_tm() - now), AVG_TIMER_WAIT ))  ; // determine delay range
    }
    
    _events.erase( _events.begin() ) ;
    _args.init() ; // hard clear; ok since all args being used will go to union 
    _args << (int32_t)now << evt.id() << (void*)&req_stop ;
    req_stop = false ; // recipient can set this to true and the timedevent will be unscheduled
    
    try 
    {
      switch (evt.trigger( _args, now ))
      {
        case  0 : // trigger ok, re-insert and keep looping
          if (req_stop) 
            delete evt.cb() ;
          else 
            _events.insert( evt ) ;
          break ;
        case -2 : // not ready yet, re-insert and drop out
          if (req_stop) 
            delete evt.cb() ;
          else 
          {
            _events.insert( evt ) ;
            return max( MIN_TIMER_DELAY, min( (long)now - (long)((*_events.begin()).next_tm()), AVG_TIMER_WAIT )) ;
          }
          break ;
        case -1 : // already expired/stopped
          delete evt.cb() ;
          break ;
        case -3 : // oneshot expired
          delete evt.cb() ;
          break ;
        case -4 : // exception caught
          delete evt.cb() ;
          break ;
      }
    } 
    catch(...)
    {
      nexceptions++ ;
    }
  } 

  return AVG_TIMER_WAIT ; // default timer wait
} // GOTimer :: action

int16_t GOTimer :: remove_the_dead() 
{
  if (_dead_timers.size() == 0)
    return 0 ;

  MutexScope                 sc( _dead_timers.gate() ) ;
  TimedEventSet_iter         it ;
  GOVector<HTIMER>::iterator d_it ;

  for (d_it = _dead_timers.begin(); d_it != _dead_timers.end(); d_it++)
  {
    for (it = _events.begin(); it != _events.end(); it++)
    {
      if ((*it).id() == (*d_it))
      {
        delete (*it).cb() ;
        _events.erase( it ) ;
        break ;
      }
    }
  }

  _dead_timers.clear() ;
  return 1 ;
} // GOTimer :: remove_the_dead

void GOTimer :: timer_proc() 
{
  _delay = 0 ;
  
  int32_t  dt ;
  while (!_done)
  {
    _delay = action( timeGetTime() ) ;
    if (!_done)
    {
      dt = min( 1000, (_delay * 1000 + _drift) / 1000 ) ;
#ifdef WIN32
      si_sleep_milli(max( 1, dt )) ;
#else
      usleep( max( 1, dt * 1000 )) ;
#endif
    }
  }
} // GOTimer :: timer_proc

GOTimer :: GOTimer() 
: _timer_t( new PokeObserver<GOTimer>( this, &GOTimer::timer_proc ) )
{
  _done  = false ;
  _drift = 0 ;
  nUsed++ ;
  nActive++ ;
} // GOTimer [constructor]

GOTimer :: ~GOTimer() 
{
  stop() ;
  clear() ;
  nActive-- ;
} // GOTimer [destructor]

void GOTimer :: clear()
{
  TimedEventSet_iter  it ;
  MutexScope          sc( _gate ) ;
  for (it = _events.begin(); it != _events.end(); it++)
  {
    delete (*it).cb() ;
  }
  _events.clear() ;
} // GOTimer :: clear

#include <stdio.h>
void GOTimer :: report()
{
  MutexScope  sc(_gate) ;

  printf( "[timer.%p.report] nEvents(%ld) delay(%d) drift(%d) thread(0x%lx active:%s) done(%s)  nTripped(%d)  TimedEvent( %d / %d )\n", this, 
          _events.size(), 
          _delay,
          _drift,
          _timer_t.handle(),
          (_timer_t.isActive()?"T":"F"), 
          (_done?"T":"F"),
          nTripped,
          TimedEvent::nActive, TimedEvent::nUsed ) ;

  TimedEventSet_iter  it ;
  for (it = _events.begin(); it != _events.end(); it++)
  {
    printf( "[timer.%p.report] cb(%p) delay(%d) one-shot(%s) id(%d)\n", this, (*it).cb(), (*it).delay(), (*it).oneshot()?"T":"F", (*it).id() ) ;
  }
} // GOTimer :: report

size_t GOTimer :: size() const 
{
  return _events.size() ;
} // GOTimer :: size

void GOTimer :: start() 
{
  _done = false ;
  _timer_t.start() ;
} // GOTimer :: start

void GOTimer :: stop()
{
  _done = true ;
  _timer_t.join() ;
  _timer_t.stop() ;
} // GOTimer :: stop

HTIMER GOTimer :: notify( uint32_t delay_ms, Callback *cb, bool oneshot )
{
  if ((cb == nullptr) || (delay_ms > MAX_TIMER_DELAY))
  {
    return -1 ;
  }

  MutexScope  sc( _gate ) ;
  HTIMER      id = _event_ticker++ ;

  _events.insert( TimedEvent( id, cb, delay_ms, 0, oneshot )) ;
  return id ;
} // GOTimer :: notify

int16_t GOTimer :: unnotify( HTIMER id, bool ok2del ) 
{
  if (id == -1)
    return -1 ; // invalid handle

  MutexScope  sc( _dead_timers.gate() ) ;
  _dead_timers.push_back( id ) ;
  return 0 ;
/*
//  MutexScope           sc( _gate ) ;
  TimedEventSet_iter   it ;

  for (it = _events.begin(); it != _events.end(); it++)
  {
    if ((*it).id() == id)
    {
      ((TimedEvent&)(*it)).stop( ok2del ) ; // not sure why compiler forces the cast to non-const

//      if (ok2del)
//      {
//        ((TimedEvent&)(*it)).set_cb( nullptr ) ;
//      }
//      _events.erase( it ) ;
      return 0 ;
    }
  }
  return -2 ; // not found
*/
} // GOTimer :: unnotify

int16_t GOTimer :: unnotify( void *dest ) 
{
  MutexScope           sc( _gate ) ;
  TimedEventSet_iter   it ;

  for (it = _events.begin(); it != _events.end(); it++)
  {
    if ((*it).cb()->destObj() == dest)
    {
      ((TimedEvent&)(*it)).stop() ; // not sure why compiler forces the cast to non-const
    }
  }
  return 0 ;
} // GOTimer :: unnotify

} // namespace go 