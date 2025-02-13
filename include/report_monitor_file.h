/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       report_monitor_file.h
  @brief      ReportMonitorFile class definition

  @author     rmcinnis@solidice.com
  @date       august 05, 1999
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#ifndef REPORT_MONITOR_FILE_H
#define REPORT_MONITOR_FILE_H

#include <report_monitor.h>

namespace go {

/*!
  @class      ReportMonitorFile
   
  <b>Description:</b>

  <b>Notes:</b>
   
  @author     rmcinnis@solidice.com 
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
class ReportMonitorFile : public ReportMonitor
{
  private :
    FILE             *_outf ;
    GOString          _fname ;
    
  public  :
                      ReportMonitorFile( const char *fname, int32_t l = 3, int32_t f = rmREPORTALL ) ;
    virtual          ~ReportMonitorFile() ;
    
    virtual void      report( ReportMsg &msg ) ;
    virtual int16_t     setFile( const char *fname ) ;

    // access methods
    const GOString   &fname() { return _fname ; }
} ; // class ReportMonitorFile

extern ReportMonitorFile &gbl_reportMonitorFile() ;

} // namespace go 

#endif

