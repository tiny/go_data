/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       pointintime.cpp
  @brief      PointInTime class implementation

  @author     rmcinnis@solidice.com
  @date       february 19, 1999
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#include <pointintime.h>
#ifdef __WIN32__
#  define  tzset  _tzset
#  include <stdio.h>
#  include <stdlib.h> // for abs
#  include <string.h> // for memcpy
#  include <time.h>
#else
#  include <time.h>
#  include <sys/time.h>
#  include <unistd.h>
#  include <stdlib.h> // for abs
#  include <string.h> // for memcpy
#  include <stricmp.h>
#  include <stdio.h>
#  ifdef FREEBSD
   extern "C" {
   int32_t ftime( struct timeb *tp ) ; // man pages say its obsoleted 
   } ;
   int32_t ftime( struct timeb *tp ) // man pages say its obsoleted 
   {
     struct timeval  tv ;
     struct timezone tz ;
     int32_t             rc ;
     if (tp == nullptr) return -1 ;
     rc = gettimeofday( &tv, &tz ) ;
     tp->time = tv.tv_sec ;
     tp->millitm = tv.tv_usec / 1000 ;
     tp->timezone = tz.tz_minuteswest ;
     tp->dstflag = tz.tz_dsttime ;
     return (tp->dstflag == 0) ? 0 : 1 ;
   }
#  endif
#endif

namespace go {

//------------------------------------------------------------------------
bool  gbl_inittimezone = false ;

const char *gbl_Months[] = 
{ "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", 
  "SEP", "OCT", "NOV", "DEC", nullptr 
} ;

const char *gbl_MonthNames[] = 
{ "January", "February", "March", "April", "May", "June", "July",
  "August", "September", "October", "November", "December", nullptr 
} ;

const char *gbl_Weekdays[] =
{
  "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", nullptr
} ;

struct GBLTimeZone
{
  int16_t     min ; 
  int16_t     dst ; 
  const char *tz_name ;
  const char *tz_desc ;
} ;


GBLTimeZone  gbl_timeZone[] =
{
  { 0,   0, "GMT",  "Greenwich Mean Time" },
  { 60,  1, "BDT" , "Bering Daylight Time"},
  { 60,  0, "BST" , "Bering Standard Time"},
  { 120, 1, "MADT", "Mid-Atlantic Daylight Time" },
  { 120, 0, "MAST", "Mid-Atlantic Standard Time" },
  { 180, 1, "NDT",  "New Foundland Daylight Time" },
  { 180, 0, "NST",  "New Foundland Standard Time" },
  { 240, 1, "ADT" , "Atlantic Daylight Time"},
  { 240, 0, "AST" , "Atlantic Standard Time"},
  { 300, 1, "EDT",  "Eastern Daylight Time" },
  { 300, 0, "EST",  "Eastern Standard Time" },
  { 360, 1, "CDT" , "Central Daylight Time"},
  { 360, 0, "CST" , "Central Standard Time"},
  { 420, 1, "MDT" , "Mountain Daylight Time"},
  { 420, 0, "MST" , "Mountain Standard Time"},
  { 480, 1, "PDT" , "Pacific Daylight Time"},
  { 480, 0, "PST" , "Pacific Standard Time"},
  { 540, 1, "AKDT", "Alaskan Daylight Time" },
  { 540, 0, "AKST", "Alaskan Standard Time" },
  { 600, 1, "HDT",  "Hawaiian Daylight Time" },
  { 600, 0, "HST",  "Hawaiian Standard Time" },
  { -1,  0, "UNK",  "Unknown Time Zone" }
};

GBLTimeZone *get_TZDef( int16_t timezone, int16_t dst ) 
{
  uint16_t i = 0 ;
  while (gbl_timeZone[i].min != -1)
  {
    if ((gbl_timeZone[i].min == timezone) && (gbl_timeZone[i].dst == dst))
      return &gbl_timeZone[i] ;
    i++ ;
  }
  return &gbl_timeZone[i]  ;
} // :: get_TZDef

const char *get_TZAbbr( int16_t timezone, int16_t dst ) 
{
  return get_TZDef( timezone, dst )->tz_name ;  
} // :: get_TZAbbr

const char *get_TZDesc( int16_t timezone, int16_t dst ) 
{
  return get_TZDef( timezone, dst )->tz_desc ;  
} // :: get_TZDesc

int16_t get_TZMinutes( const char *tz, int16_t &dst )
{
  if (tz == nullptr)
  {
    dst = 0 ;
    return 0 ;
  }
  uint16_t i = 0 ;
  while (gbl_timeZone[i].min != -1)
  {
    if (stricmp( gbl_timeZone[i].tz_name, tz ) == 0)
    {
      dst = gbl_timeZone[i].dst ;
      return gbl_timeZone[i].min ;
    }
    i++ ;
  }
  dst = 0 ;
  return 0 ;
} // :: get_TZMinutes

void timezone_init()
{
  if (!gbl_inittimezone)
  {
    gbl_inittimezone = true ;
    tzset() ;
  }
} // :: timezone_init

int16_t PointInTime :: local_tz( int16_t &dst ) 
{
#ifdef LINUX
  struct timeval  tv ;
  struct timezone tz ;

  std::gettimeofday(&tv,&tz);
  dst = tz.tz_minuteswest;
  return tz.tz_minuteswest;
#else
  struct timeb   pt ;
  ::ftime( &pt ) ;
  dst = pt.dstflag ;
  return pt.timezone ;
#endif
} // PointInTime :: local_tz 

PointInTime :: PointInTime( time_t tm, int16_t ms, int16_t tz, int16_t dstflag )
{
  switch (tm)
  {
    case TSNOW : // get current - default constructor
      getCurrent() ; // will at least initialize fields
      break ;
    case TSNULL : // null constructor
      _pt.time = TSNULL ;
      _pt.millitm = 0 ;
      _pt.timezone = tz ;
      _pt.dstflag = dstflag ;
      break ;
    default :
      _pt.time = tm - tz*60 - (dstflag * 60*60) ;
      _pt.millitm = ms ;
      _pt.timezone = tz ;
      _pt.dstflag = dstflag ;
      break ;
  }
} // PointInTime [constructor]

PointInTime :: PointInTime( const char *tz, uint16_t month, uint16_t mday, uint16_t yr, uint16_t hour, uint16_t minute, uint16_t sec, uint16_t ms ) 
{
  struct tm  tm ;
  int16_t      dst ;
  tm.tm_mon  = (month == 0) ? 0 : month-1 ; // 0..11
  tm.tm_mday = mday ; // 1..31
  tm.tm_year = (yr >= 1900) ? yr - 1900 : yr ;
  tm.tm_hour = hour ; // 0..23
  tm.tm_min  = minute ; // 0..59
  tm.tm_sec  = sec + (ms / 1000) ; // 0..59

  _pt.time     = mktime( &tm ) - local_tz( dst )*60 ;
  _pt.millitm  = ms % 1000 ;
  if (tz == nullptr)
    _pt.timezone = local_tz( _pt.dstflag ) ;
  else
  {
    _pt.timezone  = get_TZMinutes( tz, _pt.dstflag ) ;
    // find difference between tz and local_tz
//    _pt.time     += (_pt.timezone - local_tz( dst ))*60 ;
  }
} // PointInTime [constructor]

PointInTime :: PointInTime( const PointInTime &p )
{
  *this = p ;
} // PointInTime [constructor]

PointInTime :: PointInTime( const struct timeb &p )
{
  *this = p ;
} // PointInTime [constructor]

PointInTime :: ~PointInTime()
{
} // PointInTime [destructor]

void PointInTime :: getCurrent()
{
#ifdef LINUX
  struct timeval  tv ;
  struct timezone tz ;
  struct tm       tyme, utc ;

  // this call will get the milliseconds (micro seconds for linux)
  ::gettimeofday( &tv, &tz ) ;
  _pt.time = tv.tv_sec ;
  _pt.millitm = tv.tv_usec / 1000 ;
  //  _pt.timezone = tz.tz_minuteswest ; // doesn't seem to work correctly, thereby requiring the work around
  //  _pt.dstflag = tz.tz_dsttime ; // obsolete

  // work around: get UTC and local time, subtract to determine 'minuteswest'
  utc = *gmtime( &_pt.time ) ;
  tyme = *localtime( &_pt.time ) ;
  _pt.dstflag = tyme.tm_isdst ;
  _pt.timezone = (mktime( &utc ) - mktime( &tyme )) / 60 ; 
  
  _pt.time = _pt.time - _pt.timezone*60 + (_pt.dstflag * 60*60) ;
//  printf( "get current: tz(%d) dst(%d)  linux", _pt.timezone, _pt.dstflag ) ;
//  printf( " (%s %s %ld %d %d)\n", tzname[0], tzname[1], timezone, daylight, tyme.tm_isdst ) ;
#else
  ::ftime( &_pt ) ;
  _pt.time = _pt.time - _pt.timezone*60 + (_pt.dstflag * 60*60) ;
#endif
} // PointInTime :: getCurrent

bool PointInTime :: operator== ( const PointInTime &pt ) const 
{
  PointInTime  g1( GMT() ), g2( pt.GMT() ) ;
  return ((g1._pt.time != g2._pt.time) ? false : (g1._pt.millitm == g2._pt.millitm)) ;
} // PointInTime :: operator==

bool PointInTime :: operator!= ( const PointInTime &pt ) const 
{
  PointInTime  g1( GMT() ), g2( pt.GMT() ) ;
  return ((g1._pt.time == g2._pt.time) ? (g1._pt.millitm != g2._pt.millitm) : true) ;
} // PointInTime :: operator!=

bool PointInTime :: operator< ( const PointInTime &pt ) const 
{
  PointInTime  g1( GMT() ), g2( pt.GMT() ) ;
  return ((g1._pt.time > g2._pt.time) ? false : ((g1._pt.time < g2._pt.time) ? true : (g1._pt.millitm < g2._pt.millitm))) ;
} // PointInTime :: operator<

bool PointInTime :: operator<=( const PointInTime &pt ) const 
{
  PointInTime  g1( GMT() ), g2( pt.GMT() ) ;
  return ((g1._pt.time > g2._pt.time) ? false : ((g1._pt.time < g2._pt.time) ? true : (g1._pt.millitm <= g2._pt.millitm))) ;
} // PointInTime :: operator<=

bool PointInTime :: operator> ( const PointInTime &pt ) const 
{
  PointInTime  g1( GMT() ), g2( pt.GMT() ) ;
  return ((g1._pt.time < g2._pt.time) ? false : ((g1._pt.time > g2._pt.time) ? true : (g1._pt.millitm > g2._pt.millitm))) ;
} // PointInTime :: operator>

bool PointInTime :: operator>=( const PointInTime &pt ) const 
{
  PointInTime  g1( GMT() ), g2( pt.GMT() ) ;
  return ((g1._pt.time < g2._pt.time) ? false : ((g1._pt.time > g2._pt.time) ? true : (g1._pt.millitm >= g2._pt.millitm))) ;
} // PointInTime :: operator>=

bool PointInTime :: within( const PointInTime &pt, time_t nsec, uint16_t ms ) const
{
  PointInTime   diff( *this - pt ) ;
  if (diff.time() < nsec)  return true ;
  else if (diff.time() > nsec) return false ;
  else return (diff.millitm() <= ms) ;
} // PointInTime :: within

PointInTime &PointInTime :: operator= ( time_t t ) 
{
  _pt.time = t ;
  _pt.millitm = 0 ;
  // what should TZ and DST be set to??
  return *this ;
} // PointInTime :: operator=

PointInTime &PointInTime :: operator= ( const PointInTime &pt ) 
{
  ::memcpy( &_pt, &pt._pt, sizeof(_pt) ) ;
//  _pt = pt._pt ; // bitwise copy
  return *this ;
} // PointInTime :: operator=

PointInTime &PointInTime :: operator= ( const struct timeb &pt ) 
{
  ::memcpy( &_pt, &pt, sizeof(_pt) ) ;
//  _pt = pt ; // bitwise copy
  return *this ;
} // PointInTime :: operator=

PointInTime &PointInTime :: operator+= ( time_t t ) 
{
  _pt.time += t ;
  return *this ;
} // PointInTime :: operator+=

PointInTime &PointInTime :: operator+= ( int16_t ms ) 
{
  _pt.time += (ms + _pt.millitm) / 1000 ;
  _pt.millitm = (ms + _pt.millitm) % 1000 ;
  return *this ;
} // PointInTime :: operator+=

PointInTime &PointInTime :: operator+= ( const PointInTime &pt ) 
{
  _pt.time += pt._pt.time + ((_pt.millitm + pt._pt.millitm) / 1000) ;
  _pt.millitm = (pt._pt.millitm + _pt.millitm) % 1000 ;
  return *this ;
} // PointInTime :: operator+=

PointInTime PointInTime :: operator+ ( time_t t ) const
{
  struct timeb p = _pt ;
  p.time += t ;
  return PointInTime( p ) ;
} // PointInTime :: operator+

PointInTime PointInTime :: operator+ ( int16_t ms ) const
{
  struct timeb p = _pt ;
  p.time    += (ms + _pt.millitm) / 1000 ;
  p.millitm  = (ms + _pt.millitm) % 1000 ;
  return PointInTime( p ) ;
} // PointInTime :: operator+

PointInTime PointInTime :: operator+ ( const PointInTime &pt ) const
{
  PointInTime  g1( GMT() ), g2( pt.GMT() ) ;
  g1 += g2 ;
  g1.set_tz( PointInTime::get_TZAbbr(timezone(), dstflag()) ) ;
  return g1 ;
} // PointInTime :: operator+

PointInTime PointInTime :: operator- ( time_t t ) const
{
  struct timeb p = _pt ;
  p.time -= t ;
  return PointInTime( p ) ;
} // PointInTime :: operator-

PointInTime PointInTime :: operator- ( int16_t ms ) const
{
  struct timeb p = _pt ;
  if (ms > _pt.millitm)
  {
    p.time-- ;
    p.millitm = p.millitm + 1000 - ms ;
  }
  else
    p.millitm = p.millitm - ms ;
  return PointInTime( p ) ;
} // PointInTime :: operator-

PointInTime PointInTime :: operator- ( const PointInTime &pt ) const
{
  PointInTime  g1( GMT() ), g2( pt.GMT() ) ;
  time_t       t ;
  int16_t        m = 0 ;
  // subtract a time from another time yields a time difference
  t = abs( g1._pt.time - g2._pt.time ) ;
  if (g2._pt.millitm > g1._pt.millitm)
  {
    if (t > 0)
    {
      t-- ;
      m = 1000 + (g1._pt.millitm - g2._pt.millitm) ;
    }
    else
      m = (int16_t)abs(g1._pt.millitm - g2._pt.millitm) ;
  }
  else
    m = (g1._pt.millitm - g2._pt.millitm) ;
  return PointInTime( t, m, 0, 0 ) ; 
} // PointInTime :: operator-

PointInTime PointInTime :: operator* ( int16_t scalar ) const
{
  return PointInTime( _pt.time * scalar + (_pt.millitm * scalar) / 1000, _pt.millitm * scalar % 1000, _pt.timezone, _pt.dstflag ) ;
} // PointInTime :: operator*

PointInTime &PointInTime :: operator-= ( time_t t ) 
{
  _pt.time -= t ;
  return *this ;
} // PointInTime :: operator-=

PointInTime &PointInTime :: operator-= ( int16_t ms ) 
{
  if (ms > _pt.millitm)
  {
    _pt.time-- ;
    _pt.millitm = 1000 - (ms - _pt.millitm) ;
  }
  else
    _pt.millitm = _pt.millitm - ms ;
  return *this ;
} // PointInTime :: operator-=

PointInTime &PointInTime :: operator-= ( const PointInTime &pt ) 
{
  PointInTime  diff( *this - pt ) ;
  _pt.time -= diff._pt.time ;
  if (diff._pt.millitm > _pt.millitm)
  {
    _pt.time-- ;
    _pt.millitm = 1000 - (diff._pt.millitm - _pt.millitm) ;
  }
  else
    _pt.millitm = _pt.millitm - diff._pt.millitm ;  
  return *this ;
} // PointInTime :: operator-=

PointInTime &PointInTime::addMS( uint32_t ms_ )
{
  ms_ += _pt.millitm;
  _pt.time += (ms_ / 1000);
  _pt.millitm = (uint16_t)(ms_ % 1000);
  return( *this );
} // PointInTime :: addMS

const char *PointInTime :: as_string( char *dest, uint16_t /* maxsz */, uint16_t format ) const
{
  if (dest == nullptr)  return nullptr ;
  dest[0] = '\0' ;
  if ((format == 0) || isNull() || (time() < 0))  return dest ;

  char         *ch = dest ;
  PointInTime   gmt( GMT() ) ;
  time_t        tyme = time() ;
  struct tm    *tm = gmtime( &tyme ) ;

  if (format & tmfmtDATE)
  {
    if (format & tmfmt2DIGITYEAR)
    {
      if (format & tmfmtDATEFMT2) /* FEB 01, 2000 */
        ch += sprintf( ch, "%s %02d, %02d", gbl_Months[tm->tm_mon], tm->tm_mday, (tm->tm_year+1900) % 100 ) ;
      else if (format & tmfmtDATEFMT3) /* February 01, 2000 */
        ch += sprintf( ch, "%s %02d, %02d", gbl_MonthNames[tm->tm_mon], tm->tm_mday, (tm->tm_year+1900) % 100 ) ;
      else if (format & tmfmtDATEFMT4) /* 01 FEB 2000 */
        ch += sprintf( ch, "%02d %s %02d", tm->tm_mday, gbl_Months[tm->tm_mon], (tm->tm_year+1900) % 100 ) ;
      else /* tmfmtDATEFMT1 02-01-2000 */  /*  DEFAULT   */
        ch += sprintf( ch, "%02d-%02d-%02d", tm->tm_mon+1, tm->tm_mday, (tm->tm_year+1900) % 100 ) ;
    }
    else
    {
      if (format & tmfmtDATEFMT2) /* FEB 01, 2000 */
        ch += sprintf( ch, "%s %02d, %04d", gbl_Months[tm->tm_mon], tm->tm_mday, tm->tm_year+1900 ) ;
      else if (format & tmfmtDATEFMT3) /* February 01, 2000 */
        ch += sprintf( ch, "%s %02d, %04d", gbl_MonthNames[tm->tm_mon], tm->tm_mday, tm->tm_year+1900 ) ;
      else if (format & tmfmtDATEFMT4) /* 01 FEB 2000 */
        ch += sprintf( ch, "%02d %s %04d", tm->tm_mday, gbl_Months[tm->tm_mon], tm->tm_year+1900 ) ;
      else if (format & tmfmtDATEFMT5) /* dd-mm-yyyy */
        ch += sprintf( ch, "%02d-%02d-%04d", tm->tm_mday, tm->tm_mon+1, tm->tm_year+1900 ) ;
      else /* tmfmtDATEFMT1 02-01-2000 */  /*  DEFAULT   */
        ch += sprintf( ch, "%02d-%02d-%04d", tm->tm_mon+1, tm->tm_mday, tm->tm_year+1900 ) ;
    }
  }

  if (format & tmfmtTIME)
  {
    if (format & tmfmtDATE)
    {
      strcpy( ch, " " ) ;
      ch++ ;
    }
    char  blink = ((format & tmfmtBLINKSEP) && (tm->tm_sec & 0x01)) ? ' ' : ':' ;
    if (format & tmfmt24HOUR)
      ch += sprintf( ch, "%02d%c%02d", tm->tm_hour, blink, tm->tm_min ) ;
    else
      ch += sprintf( ch, "%02d%c%02d", 
                    (tm->tm_hour > 12) ? tm->tm_hour-12 : ((tm->tm_hour == 0)?12:tm->tm_hour), 
                    blink, tm->tm_min ) ;
    if (format & tmfmtSHOWSEC)
      ch += sprintf( ch, "%c%02d", blink, tm->tm_sec ) ;
    if (format & tmfmtSHOWMS)
      ch += sprintf( ch, ".%03d", millitm() ) ;
    if ((format & tmfmt24HOUR) == 0)
      ch += sprintf( ch, "%c", (tm->tm_hour >= 12) ? 'p' : 'a' ) ;
    if (format & tmfmtTZ)
    {
      GBLTimeZone  *tz = get_TZDef( timezone(), dstflag() ) ;
      if (tz->min == -1) // unknown - use GMT+hh
      {
        bool isEast = (timezone() < 0) ;
        ch += sprintf( ch, " GMT%c%02d:%02d", (isEast ? '+' : '-'), abs(timezone()) / 60, abs(timezone()) % 60 ) ;
      }
      else
        ch += sprintf( ch, " %s", tz->tz_name ) ;
    }
  }

  return dest ;
} // PointInTime :: as_string

int32_t PointInTime :: diffMilli( const PointInTime &pt ) const 
{ 
  PointInTime t( *this - pt ) ;
  return t._pt.time*1000 + t._pt.millitm ;
} // PointInTime :: diffMilli

PointInTime PointInTime :: GMT() const 
{
  return PointInTime( time() + timezone()*60, millitm(), 0, false ) ;
} // PointInTime :: GMT

int16_t PointInTime :: set_tz( const char *tz ) 
{
  if (tz == nullptr)
    return -1 ;
  int16_t   dst, nmin = get_TZMinutes( tz, dst ) ;

  _pt.time     += (_pt.timezone - nmin) * 60 ;
  _pt.timezone  = nmin ;
  _pt.dstflag   = dst ;

  return 0 ;
} // PointInTime :: set_tz

PointInTime &PointInTime :: subtractMS( uint32_t ms_ ){
  int32_t seconds = ms_ / 1000;
  uint16_t ms = (uint16_t)(ms_ % 1000);
  if( ms > _pt.millitm ){
    seconds--;
    _pt.millitm += 1000;
  }
  _pt.time -= seconds;
  _pt.millitm = _pt.millitm - ms;
  return( *this );
} // PointInTime :: subtractMS

const char *PointInTime :: get_TZAbbr( int16_t timezone, uint16_t dst ) 
{
  return get_TZAbbr( timezone, dst ) ;
} // PointInTime :: get_TZAddr

const char *PointInTime :: get_TZDesc( int16_t timezone, uint16_t dst ) 
{
  return get_TZDesc( timezone, dst ) ;
} // PointInTime :: get_TZDesc

} // namespace go 
