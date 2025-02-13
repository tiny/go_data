/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       report_monitor_file.cpp
  @brief      ReportMonitorFile class definition

  @author     rmcinnis@solidice.com
  @date       august 05, 1999
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#include <report_monitor_file.h>
#ifdef FREEBSD
# include <unistd.h>
#endif

namespace go {

//ReportMonitorFile  gbl_reportMonitorFile( "stdout", -1, rmREPORTALL ) ;
ReportMonitorFile  *gbl_reportMonitorFileInstance = nullptr ;

ReportMonitorFile  &gbl_reportMonitorFile()
{
  if (gbl_reportMonitorFileInstance == nullptr)
  {
    gbl_reportMonitorFileInstance = new ReportMonitorFile( "stdout", -1, rmREPORTALL ) ;
  }
  return *gbl_reportMonitorFileInstance ;
} // :: gbl_reportMonitorFile

void gbl_reportMonitorFile_release()
{
  if (gbl_reportMonitorFileInstance == nullptr)
    return ;
  static bool  door_open = true ; // can't use mutex because i don't want to leave it around after i exit
  if (!door_open)
    return ;
  door_open = false ;  
  gbl_reportMonitorFileInstance->shutdown() ;
  delete gbl_reportMonitorFileInstance ;
  gbl_reportMonitorFileInstance = nullptr ;
  door_open = true ;
} // :: gbl_reportMonitorFile_release

extern bool gbl_isConsoleApp ; // lives in report.cpp
extern bool gbl_showreporttm ; // lives in report.cpp

//-----------------------------------------------------------------------------  
ReportMonitorFile :: ReportMonitorFile( const char *fn, int32_t l, int32_t f )
: ReportMonitor( l, f )
, _fname( (const char *)nullptr ) 
{
  _outf = nullptr ;
  setFile( fn ) ;
} // ReportMonitorFile [constructor]

ReportMonitorFile :: ~ReportMonitorFile()
{
  if ((_outf != nullptr) && (_outf != stdout) && (_outf != stderr))
  {
    ::fclose( _outf ) ;
    _outf = nullptr ;
  }
} // ReportMonitorFile [destructor]
    
void ReportMonitorFile :: report( ReportMsg &msg )
{
#ifdef _MSC_VER
  if ((_outf == nullptr) || (_outf == stdout))
  {
    // View in MSVC++ trace window or debugging console
#  ifdef UNICODE
    static TCHAR msgBufW[512];
    mbstowcs( msgBufW, (const char *)msg, msg.size() );
    OutputDebugString( msgBufW );
#  else
    OutputDebugString( (const char*)msg );
#  endif
    if (gbl_isConsoleApp)
    {
      _outf = stdout ;
      if (gbl_showreporttm)
      {
        char buf[64] ;
        ::fprintf( _outf, "%s| ", msg.tm().as_string( buf, 64, tmfmtDEFAULT | tmfmtSHOWMS )) ;
      }
      ::fprintf( _outf, (const char*)msg ) ;
      ::fflush( _outf ) ;
    }
  }
  else
  {
    if (gbl_showreporttm)
    {
      char buf[64] ;
      ::fprintf( _outf, "%s| ", msg.tm().as_string( buf, 64, tmfmtDEFAULT | tmfmtSHOWMS )) ;
    }
    ::fprintf( _outf, (const char*)msg ) ;
    ::fflush( _outf ) ;
  }
#else // _MSC_VER
  if (_outf != nullptr)
  {
    if (gbl_showreporttm)
    {
      char buf[64] ;
      ::fprintf( _outf, "%s| ", msg.tm().as_string( buf, 64, tmfmtDEFAULT | tmfmtSHOWMS )) ;
    }
    ::fprintf( _outf, "%s", (const char*)msg ) ;
    ::fflush( _outf ) ;
  }
#endif   
} // ReportMonitorFile :: report

int16_t ReportMonitorFile :: setFile( const char *fname )
{
  if (_outf != nullptr)
  {
    if ((_outf != stderr) && (_outf != stdout))
      ::fclose( _outf ) ;
    _outf = nullptr ;
  }

  _fname = fname ;
  if (fname == nullptr)
    return 0 ;

  if (_fname == "stdout")
    _outf = stdout ;
  else if (_fname == "stderr")
    _outf = stderr ;
  else if (_fname == "tmpfile")
  {
#ifdef LINUX
    _fname = "smc_XXXXXX" ;
    int32_t fd = mkstemp(_fname) ;    
    _outf = ::fdopen( fd, "w+" ) ;
#else
    _fname = tmpnam(nullptr) ;    
    _outf = ::fopen( _fname, "w+" ) ;
#endif
    //    char  buf[32] ;
    //    sprintf( buf, "smc_%ld.tmp", time(nullptr) ) ;
    //    _fname = buf ;
    //    _outf = ::fopen( _fname, "w+" ) ;
  }
  else
  {
    _outf = ::fopen( _fname, "a+" ) ;
    if (_outf == nullptr)
      _outf = ::fopen( _fname, "w+" ) ;
  }
  return 0 ;
} // ReportMonitorFile :: setFile

} // namespace go 
