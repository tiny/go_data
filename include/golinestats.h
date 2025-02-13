/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       golinestats.h
  @brief      GOLineStats class definition

  @author     rmcinnis@solidice.com
  @date       december 06, 1999
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#ifndef LINESTATS_H
#define LINESTATS_H

#include <gotimer.h>
#include <pointintime.h>
#include <gosampler.h>
#include <gonumeric.h>
#include <gostring.h>

namespace go {

/*!
  @class GOLineStats
   
  <b>Description:</b>
   
  <b>Notes:</b>
   
  @author     rmcinnis@solidice.com 
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
class GOLineStats
{
  private  :
    GOTimer            *_timer ;
    HTIMER              _hTimer ;
    PointInTime         _tmOpen ;
    PointInTime         _tmClose ;
    PointInTime         _tmLast ;   // time of last transaction
    GOLong              _duration ; // number of milliseconds the connection has been up (now - tmOpen, set on timer event)
    GOSampler           _latency ;
    GOLong              _nBytesRecv ;
    GOLong              _nBytesSent ;
    GOLong              _nBytesBad ;
    GOLong              _nTicksRecv ;
    GOLong              _nTicksSent ;
    GOString            _desc ;
    // rate calculations
    GODouble            _rBS ; // received kbytes/sec
    GODouble            _sBS ; // sent     kbytes/sec
    GODouble            _rTS ; // received ticks/sec
    GODouble            _sTS ; // sent     ticks/sec
    PointInTime         _tmInterval ; // intrnal for rate calculations
    uint32_t               _rBytes ;
    uint32_t               _sBytes ;
    uint32_t               _rTick ;
    uint32_t               _sTick ;
    GOSampler           _recvRateSamples ;
    GOSampler           _sentRateSamples ;

    virtual void        onRateCheckInterval() ;
    
  public   :
                        GOLineStats( GOTimer *timer_, const char *des, uint16_t sampleSz = 10 ) ;
    virtual            ~GOLineStats() ;

    virtual void        timer_hookup() ;
    virtual void        timer_unhook() ;

    // access methods
    PointInTime        &tmOpen() { return _tmOpen ; }
    PointInTime        &tmClose() { return _tmClose ; }
    PointInTime        &tmLast() { return _tmLast ; }
    GOLong             &duration() { return _duration ; }
    GOSampler          &latency() { return _latency ; }
    GOLong             &nBytesRecv() { return _nBytesRecv ; }
    GOLong             &nBytesSent() { return _nBytesSent ; }
    GOLong             &nBytesBad() { return _nBytesBad ; }
    GOLong             &nTicksRecv() { return _nTicksRecv ; }
    GOLong             &nTicksSent() { return _nTicksSent ; }
    GOString           &desc() { return _desc ; }
    GODouble           &rBS() { return _rBS ; }
    GODouble           &sBS() { return _sBS ; }
    GODouble           &rTS() { return _rTS ; }
    GODouble           &sTS() { return _sTS ; }
    GOSampler          &recvRateSamples() { return _recvRateSamples ; }
    GOSampler          &sentRateSamples() { return _sentRateSamples ; }
} ; // class GOLineStats

} // namespace go 

#endif

