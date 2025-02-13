/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       ptint32_tm.cpp
  @brief      PtInTm structure implementation

  @author     rmcinnis@solidice.com
  @date       april 29, 2000
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#include <ptintm.h>

#ifdef UNIX
#  include <netinet/in.h>
#elif defined(_AFXDLL) || (defined(MINGW) && defined(WIN32))
#  ifndef _WINSOCK2API_
#    include <winsock2.h>
#  endif
#elif defined(WIN32)
#  include <winsock2.h>
#endif

namespace go {

PtInTm :: PtInTm()
{
  clear() ;
} // PtInTm [constructor]

PtInTm :: PtInTm( const PtInTm &t )
{
  *this = t ;
} // PtInTm [constructor]

PtInTm :: PtInTm( const PointInTime &t )
{
  *this = t ;
} // PtInTm [constructor]

void PtInTm :: clear()
{
  tm = 0 ;
  ms = 0 ;
  dst_tz = 0 ;
} // PtInTm :: clear

void PtInTm :: decodeHost()
{
  tm = ntohl(tm) ;
  ms = ntohs(ms) ;
  dst_tz = ntohs(dst_tz) ;
} // PtInTm :: decodeHost

void PtInTm :: encodeNetwork()
{
  tm = htonl(tm) ;
  ms = htons(ms) ;
  dst_tz = htons(dst_tz) ;
} // PtInTm :: encodeNetwork

bool PtInTm :: isNull()
{
  return ((tm == 0) && (ms == 0) && (dst_tz == 0)) ;
} // PtInTm :: isNull

  // operator overloads
PtInTm &PtInTm :: operator= ( const PtInTm &t ) 
{
  tm = t.tm ;
  ms = t.ms ;
  dst_tz = t.dst_tz ;
  return *this ;
} // PtInTm :: operator=

PtInTm &PtInTm :: operator= ( const PointInTime &t ) 
{
  tm = ((PointInTime&)t).time() ;
  ms = ((PointInTime&)t).millitm() ; 
  dst_tz = ((PointInTime&)t).pt().timezone ; 
  if (((PointInTime&)t).pt().dstflag)
    dst_tz |= 0x8000 ;
  return *this ;
} // PtInTm :: operator=

bool PtInTm :: operator==( const PtInTm &t ) const
{
  return (((PointInTime)*this) == (PointInTime)t) ;
} // PtInTm :: operator==

bool PtInTm :: operator!=( const PtInTm &t ) const 
{ return !(*this == t) ;
} // PtInTm :: operator!=

bool PtInTm :: operator<( const PtInTm &t ) const
{ return (((PointInTime)*this) < (PointInTime)t) ;
} // PtInTm :: operator<

bool PtInTm :: operator<=( const PtInTm &t ) const
{ return (((PointInTime)*this) <= (PointInTime)t) ;
} // PtInTm :: operator<=

bool PtInTm :: operator>( const PtInTm &t ) const
{
  return (((PointInTime)*this) > (PointInTime)t) ;
} // PtInTm :: operator>

bool PtInTm :: operator>=( const PtInTm &t ) const
{
  return (((PointInTime)*this) >= (PointInTime)t) ;
} // PtInTm :: operator>=

PtInTm :: operator PointInTime() const 
{
  struct timeb t ;
  t.time = tm ; t.millitm = ms ; 
  t.timezone = (dst_tz & 0x7FFF) ; t.dstflag = ((dst_tz & 0x8000) != 0) ;
  return PointInTime( t ) ; 
} // PtInTm :: operator PointInTime

} // namespace go 

