/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       pointintime.h
  @brief      PointInTime class definition

  @author     rmcinnis@solidice.com
  @date       february 19, 1999
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#ifndef POINTINTIME_H
#define POINTINTIME_H

#include <enum.h>
#include <time.h>
#include <sys/timeb.h> // for timeb struct definition

/* defined in sys/timeb.h
   struct _timeb 
   {
     time_t  time ;
     uint16_t  millitm ;
     int16_t   timezone ;
     int16_t   dstflag ;
   } ;

   defined in time.h
   struct tm 
   {
     int32_t tm_sec ;     // seconds after the minute - [0,59] 
     int32_t tm_min ;     // minutes after the hour - [0,59] 
     int32_t tm_hour ;    // hours since midnight - [0,23] 
     int32_t tm_mday ;    // day of the month - [1,31] 
     int32_t tm_mon ;     // months since January - [0,11] 
     int32_t tm_year ;    // years since 1900 
     int32_t tm_wday ;    // days since Sunday - [0,6] 
     int32_t tm_yday ;    // days since January 1 - [0,365] 
     int32_t tm_isdst ;   // daylight savings time flag 
   } ;
*/

// DEFAULT   mm:hh:ss EST
// add date  mm-dd-yyyy  mm:hh:ss EST
#define tmfmt24HOUR               0x0001 /* 13:00:00 ; 01:00:00p  DEFAULT */
#define tmfmtDATE                 0x0002
#define tmfmtTIME                 0x0004
#define tmfmtSHOWSEC              0x0008 /* :00 */
#define tmfmtSHOWMS               0x0010 /* :00.000 */
#define tmfmtBLINKSEP             0x0020 /* effect - blink ':' on odd seconds */
#define tmfmtDAYOFWEEK            0x0040
#define tmfmtTZ                   0x0080 /* EST */
#define tmfmt2DIGITYEAR           0x0100 /* 01 */
#define tmfmtDATEFMT1             0x0200 /* 02-01-2000 */  /*  DEFAULT   */
#define tmfmtDATEFMT2             0x0400 /* FEB 01, 2000 */
#define tmfmtDATEFMT3             0x0800 /* February 01, 2000 */
#define tmfmtDATEFMT4             0x1000 /* 01 FEB 2000 */
#define tmfmtDATEFMT5             0x2000 /* dd-mm-yyyy  */
#define tmfmtDEFAULT              (tmfmtTIME | tmfmt24HOUR | tmfmtSHOWSEC | tmfmtTZ)

namespace go {

/*!
  @class PointInTime
   
  <b>Description:</b>

  <b>Notes:</b>
   
  @author     rmcinnis@solidice.com 
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
class PointInTime
{
  private   :
    struct timeb   _pt ;

    int16_t          local_tz( int16_t &dst ) ; // minutes from GMT

  public    :
    enum
    { 
      TSNULL = -2,
      TSNOW  = -1
    };
                   PointInTime( time_t tm = TSNOW, int16_t ms = 0, int16_t tz = 0, int16_t dstflag = 0 ) ;
                   PointInTime( const char *tz, uint16_t month, uint16_t day, uint16_t yr, uint16_t hour = 0, uint16_t minute = 0, uint16_t sec = 0, uint16_t ms = 0 ) ;
                   PointInTime( const PointInTime &p ) ;
                   PointInTime( const struct timeb &p ) ;
    virtual       ~PointInTime() ;
    
    virtual void   getCurrent() ;
    bool           isNull() const { return (_pt.time == TSNULL) ; }

    // comparison operators
    bool           operator==( const PointInTime &pt ) const ;
    bool           operator!= ( const PointInTime &pt )const  ;
    bool           operator< ( const PointInTime &pt ) const ;
    bool           operator<= ( const PointInTime &pt ) const ;
    bool           operator> ( const PointInTime &pt ) const ;
    bool           operator>= ( const PointInTime &pt ) const ;
    bool           within( const PointInTime &pt, time_t nsec, uint16_t ms = 0 ) const ;

    // assignment operators
    PointInTime   &operator= ( time_t t ) ;
    PointInTime   &operator= ( const PointInTime &pt ) ;
    PointInTime   &operator= ( const struct timeb &pt ) ;
    PointInTime    operator+ ( time_t t ) const ;
    PointInTime    operator+ ( int16_t ms ) const ;
    PointInTime    operator+ ( const PointInTime &pt ) const ;
    PointInTime    operator- ( time_t t ) const ;
    PointInTime    operator- ( int16_t ms ) const ;
    PointInTime    operator- ( const PointInTime &pt ) const ;
    PointInTime    operator* ( int16_t scalar ) const ;
    PointInTime   &operator+= ( time_t t ) ;
    PointInTime   &operator+= ( int16_t ms ) ;
    PointInTime   &operator+= ( const PointInTime &pt ) ;
    PointInTime   &operator-= ( time_t t ) ;
    PointInTime   &operator-= ( int16_t ms ) ;
    PointInTime   &operator-= ( const PointInTime &pt ) ;

    // cast operators
                   operator struct timeb() { return _pt ; }

    PointInTime   &addMS( uint32_t ms );
    const char    *as_string( char *dest, uint16_t maxsz, uint16_t format = tmfmtDEFAULT ) const ;
    int32_t           diffMilli( const PointInTime &pt ) const ;
    PointInTime    GMT() const ;
    bool           is_GMT() const { return (timezone() == 0) ; }
    int16_t          set_tz( const char *tz ) ; // EST, EDT, ...
    PointInTime   &subtractMS( uint32_t ms );
   
    // access methods
    struct timeb  & pt() { return _pt ; }
    time_t          time() const { return _pt.time ; }
    uint16_t          millitm() const { return _pt.millitm ; }
    int16_t           timezone() const { return _pt.timezone ; } // n minutes from GMT
    int16_t           dstflag() const { return _pt.dstflag ; }

    // statics
    static const char *get_TZAbbr( int16_t timezone, uint16_t dst ) ;
    static const char *get_TZDesc( int16_t timezone, uint16_t dst ) ;
} ; // class PointInTime

extern const char *gbl_Months[] ;
extern const char *gbl_MonthNames[] ;
extern const char *gbl_Weekdays[] ;

} // namespace go 

#endif
  
