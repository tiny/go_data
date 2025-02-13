/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       report_monitor.h
  @brief      ReportMonitor class definition

  @author     rmcinnis@solidice.com
  @date       august 05, 1999
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#ifndef REPORT_MONITOR_H
#define REPORT_MONITOR_H

#include <report.h>
#include <report_msg.h>

namespace go {

/*!
  @class      ReportMonitor
   
  <b>Description:</b>

  <b>Notes:</b>
   
  @author     rmcinnis@solidice.com 
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
class ReportMonitor
{
  protected :
    int32_t           _flags ;
    int32_t           _level ;
    bool           _enabled ;
    bool           _initialized ;
    
    virtual void   onDebugLevelChange() ;
    virtual void   onReportUpdate() ;
    
  public    :
                   ReportMonitor( int32_t level = 3, int32_t f = rmREPORTALL ) ;
    virtual       ~ReportMonitor() ;

    virtual int16_t  initialize() ;
    virtual void   report( ReportMsg &msg ) = 0 ;
    virtual void   shutdown() ;

    // access methods
    int32_t          &flags() { return _flags ; }
    int32_t          &level() { return _level ; }
    bool          &enabled() { return _enabled ; }
} ; // class ReportMonitor

} // namespace go 

#endif

