/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       report.cpp
  @brief      debug_report functional prototypes

  @author     rmcinnis@solidice.com
  @date       may 01, 1997
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#include <report.h>
#include <gonumeric.h>
#include <report_msg.h>
#include <report_monitor_file.h>

namespace go {

//----------------------------------------------------------------------------------------
GOLong  gbl_debuglevel      = 3 ;
bool    gbl_isConsoleApp    = false ;
bool    gbl_showreporttm    = false ;
bool    gbl_reportingActive = false ;

//----------------------------------------------------------------------------------------
inline bool debug_is_thread_excluded()
{
  return false ;

/* removed until ported

  int32_t       *l, self ;
  MutexScope  sc( gbl_excluded_threads.gate() ) ;

  self = (int32_t)thread_self() ;
  gbl_excluded_threads.first() ;
  while ((l = gbl_excluded_threads.next()) != nullptr)
    if (*l == self)
      break ;

  return (l != nullptr) ;
*/
} // :: debug_is_thread_excluded

void debug_register_level( int32_t id_or_this, int32_t level )
{
  report_output_str().register_level( id_or_this, level ) ;  
} // :: debug_register_level

void debug_report( int32_t level, char *format, ... )
{
  va_list     ap ;
  va_start( ap, format ) ;

  debug_report( 0L, rmREPORTALL, level, format, ap ) ;

  va_end( ap ) ;
} // :: debug_report

void debug_report( int32_t flags, int32_t level, char *format, ... )
{
  va_list     ap ;
  va_start( ap, format ) ;

  debug_report( 0L, flags, level, format, ap ) ;

  va_end( ap ) ;
} // :: debug_report

void debug_report( int32_t userdata, int32_t flags, int32_t level, char *format, va_list ap )
{
  if (!gbl_reportingActive || (report_output_str().valueCB().nWatchers() == 0))
  {
    return ;
  }
  
  if (!debug_is_thread_excluded())
  {
    report_output_str().userdata() = userdata ;
    report_output_str().flags() = flags ;
    report_output_str().level() = level ;
    if (report_output_str().isWatched())
    {
      report_output_str().tm().getCurrent() ;
      report_output_str().vsprint( format, ap ) ; // observable report output area
    }
  }
} // :: debug_report

void reporting_level( int32_t d ) 
{
  gbl_debuglevel = d ;
  debug_register_level((int64_t)&gbl_debuglevel, d ) ;
} // :: reporting_level

void gbl_reportMonitorFile_release() ;

void reporting_start( const char *outfilename, bool isConsoleApp, bool showTimes )
{
  gbl_reportingActive = true ;
  gbl_isConsoleApp    = isConsoleApp ;
  gbl_showreporttm    = showTimes ;
  if (outfilename != nullptr)
  {
    gbl_reportMonitorFile().initialize() ;
    gbl_reportMonitorFile().setFile( outfilename ) ;
  }
} // :: reporting_start

void reporting_stop() 
{
  gbl_reportingActive = false ;
  gbl_reportMonitorFile_release() ;
} // :: reporting_stop

} // namespace go 

