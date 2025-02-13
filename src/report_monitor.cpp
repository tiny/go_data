/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       report_monitor.cpp
  @brief      ReportMonitor class definition

  @author     rmcinnis@solidice.com
  @date       august 05, 1999
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#include <report_monitor.h>
#include <stdio.h>
#include <gonumeric.h>

namespace go {

extern GOLong gbl_debuglevel ; // lives in report.cpp

void ReportMonitor :: onDebugLevelChange()
{
  _level = gbl_debuglevel ;
  debug_register_level( (int64_t)this, _level ) ;
} // ReportMonitor :: onDebugLevelChange

void ReportMonitor :: onReportUpdate()
{
  if (!enabled())
    return ;
  
  if (report_output_str().level() > _level)
    return ;
  
  if (!isReported( flags(), report_output_str().flags()))
    return ;

  report( report_output_str() ) ;
} // ReportMonitor :: onReportUpdate

ReportMonitor :: ReportMonitor( int32_t level, int32_t f )
{
  _level       = level ;
  _flags       = f ;
  _enabled     = true ;
  _initialized = false ;
  if(_level > 0)
    debug_register_level( (int64_t)this, _level ) ;
} // ReportMonitor [constructor]

ReportMonitor :: ~ReportMonitor()
{
  shutdown() ;
} // ReportMonitor [destructor]

int16_t ReportMonitor :: initialize()
{
  if (_initialized)
    return 0 ;
  _initialized = true ;

  report_output_str().valueCB().install( new PokeObserver<ReportMonitor>( this, &ReportMonitor::onReportUpdate )) ;
  gbl_debuglevel.valueCB().install( new PokeObserver<ReportMonitor>( this, &ReportMonitor::onDebugLevelChange )) ;  
  onDebugLevelChange() ; // force initialization
  return 0 ;
} // ReportMonitor :: initialize

void ReportMonitor :: shutdown() 
{
  if (!_initialized)
    return ;
  _initialized = false ;
  delete report_output_str().valueCB().remove( this ) ;
  delete gbl_debuglevel.valueCB().remove( this ) ;
} // ReportMonitor :: shutdown

} // namespace go 
