/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       gostopwatch.cpp
  @brief      GOStopWatch class implementation

  @author     rmcinnis@solidice.com
  @date       january 12, 1998
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#include <gostopwatch.h>
#include <stdio.h>
#include <string.h>

namespace go {

int16_t GOStopWatch :: on_timer( const ArgList *args ) 
{
  if (!_isRunning)
  {
    if (args != nullptr)
    {
      bool   *req_stop = (bool*)args->asVoid(2) ;
      *req_stop = true ;
    }
    return 0 ;
  }

  render() ;
  return 0 ;
} // GOStopWatch :: on_timer

void GOStopWatch :: render() 
{
  int32_t   dt = _diff ;

  if (_isRunning && !_start_ts.isNull())
  {
    PointInTime  now ;

    dt += now.diffMilli( _start_ts ) ;
  }

  render_string( dt ) ;
} // GOStopWatch :: render

void GOStopWatch :: render_string( int32_t dt )
{
  char   buf[32] ;
  int16_t  nMin, nSec, nMilli ;

  nMilli = (int16_t)(dt % 1000) ;
  dt    /= 1000L ; // turn milliseconds int32_to seconds
  nSec   = (int16_t)(dt % 60) ; // strip off spare seconds
  dt    /= 60L ; // turn seconds int32_to minutes
  nMin   = (int16_t)dt ;

  sprintf_s( buf, 32, _fmt.c_str(), nMin, nSec, nMilli ) ;
  GOString::operator=( buf ) ;
} // GOStopWatch :: render_string

GOStopWatch :: GOStopWatch( GOTimer *tymer ) 
: GOString( "00:00.000" ) 
, _start_ts( PointInTime::TSNULL ) 
, _fmt( "%3d:%02d.%03d" ) 
{
  _isRunning = false ;
  _diff      = 0 ;
  _hTimer    = NULL; // _timer.notify( 1000/10, new PokeObserver<GOStopWatch>( this, &GOStopWatch::onTimer )) ;
  _timer     = tymer ;
} // GOStopWatch [constructor]

GOStopWatch :: GOStopWatch( const GOStopWatch &i ) 
: GOString( i )
, _start_ts( i._start_ts ) 
, _fmt( i._fmt ) 
{
  _isRunning = i._isRunning ;
  _diff      = i._diff ;
  _timer     = i._timer ;
  if (_isRunning && (_timer != nullptr))
  {
    _hTimer    = _timer->notify( 1000/10, new Observer<GOStopWatch>( this, &GOStopWatch::on_timer )) ;
  }
  else
  {
    _hTimer    = NULL;
  }
} // GOStopWatch [constructor]

GOStopWatch :: ~GOStopWatch() 
{
  stop() ;
} // GOStopWatch [destructor]

void GOStopWatch :: reset() 
{
  _isRunning = false ;
  _diff      = 0 ;
  _start_ts  = PointInTime( PointInTime::TSNULL ) ;

  render() ;
} // GOStopWatch :: reset

void GOStopWatch :: start() 
{
  if (_isRunning || (_timer == nullptr))
    return ;

  _hTimer = _timer->notify( 1000/10, new Observer<GOStopWatch>( this, &GOStopWatch::on_timer )) ;
  _timer->start() ;
  _isRunning = true ;
  _start_ts.getCurrent() ;

  render() ;
} // GOStopWatch :: start

void GOStopWatch :: stop() 
{
  if (!_isRunning)
    return ;

  _isRunning = false ;

  if (_hTimer != NULL)
  {
    _timer->unnotify( _hTimer ) ;
    _hTimer = NULL;
  }

  PointInTime  now ;
  _diff     += now.diffMilli( _start_ts ) ;
  _start_ts  = PointInTime( PointInTime::TSNULL ) ;

//  render() ;
} // GOStopWatch :: stop

} // namespace go 
