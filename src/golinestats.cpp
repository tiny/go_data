/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       golinestats.CPP
  @brief      GOLineStats class implementation

  @author     rmcinnis@solidice.com
  @date       december 06, 1999
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#include <golinestats.h>

namespace go {

void GOLineStats :: onRateCheckInterval() 
{
  PointInTime  now ;
  int32_t  rd, sd, dt, rt, st ;

  _duration = now.diffMilli( _tmOpen ) ;

  rd = ((int32_t)_nBytesRecv) - _rBytes ;
  sd = ((int32_t)_nBytesSent) - _sBytes ;
  rt = ((int32_t)_nTicksRecv) - _rTick ;
  st = ((int32_t)_nTicksSent) - _sTick ;

  dt = now.diffMilli( _tmInterval ) ;
  if (dt != 0L)
  {
    _rBS = ((double)rd * 1000.0) / (double)dt ; // recv bytes/second
    _recvRateSamples.data( (int32_t)_rBS ) ;
    _sBS = ((double)sd * 1000.0) / (double)dt ; // sent bytes/second
    _sentRateSamples.data( (int32_t)_sBS ) ;
    _rTS = ((double)rt * 1000.0) / (double)dt ; // recv ticks/second
    _sTS = ((double)st * 1000.0) / (double)dt ; // sent ticks/second
  }

  // set up for next time
  _tmInterval = now ;
  _rBytes = _nBytesRecv ;
  _sBytes = _nBytesSent ;
  _rTick  = _nTicksRecv ;
  _sTick  = _nTicksSent ;
} // GOLineStats :: onRateCheckInterval

GOLineStats :: GOLineStats( GOTimer *timer_, const char *des, uint16_t sampleSz )
: _timer( timer_ )
, _latency( sampleSz )
, _desc( des ) 
, _recvRateSamples( sampleSz )
, _sentRateSamples( sampleSz )
{
  _hTimer = NULL ;
  _rBytes = 0L ;
  _sBytes = 0L ;
  _rTick  = 0L ;
  _sTick  = 0L ;

  timer_hookup() ;
  if (_timer != nullptr) 
    _timer->start() ;
} // GOLineStats [constructor]

GOLineStats :: ~GOLineStats()
{
  timer_unhook() ;
} // GOLineStats [destructor]

void GOLineStats :: timer_hookup() 
{
  timer_unhook() ; // make sure not to hook up more then once
  if ((_hTimer == NULL) && (_timer != nullptr))
  {
    _hTimer = _timer->notify( 1*1000, new PokeObserver<GOLineStats>( this, &GOLineStats::onRateCheckInterval )) ;
  }
} // GOLineStats :: timer_hookup

void GOLineStats :: timer_unhook() 
{
  if ((_hTimer != NULL) && (_timer != nullptr))
  {
    _timer->unnotify( _hTimer ) ;
    _hTimer = NULL ;
  }
} // GOLineStats :: timer_unhook

} // namespace go 
