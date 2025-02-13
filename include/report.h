/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       report.h
  @brief      debug_report functional prototypes

  @author     rmcinnis@solidice.com
  @date       may 01, 1997
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#ifndef REPORT_H
#define REPORT_H

#include <report_flags.h>
#include <stdarg.h>
#include <gonumeric.h>

namespace go {

extern GOLong gbl_debuglevel ;

void debug_register_level( int32_t id_or_this, int32_t level ) ;
void debug_report( int32_t level, char *format, ... ) ;
void debug_report( int32_t flags, int32_t level, char *format, ... ) ;
void debug_report( int32_t userdata, int32_t flags, int32_t level, char *format, va_list ap ) ;
void reporting_level( int32_t d ) ;
void reporting_start( const char *outfilename = "stdout", bool isConsoleApp = true, bool showTimes = true ) ;
void reporting_stop() ;

} // namespace go 

#endif
