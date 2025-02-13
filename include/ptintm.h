/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       ptint32_tm.h
  @brief      PtInTm structure definition

  @author     rmcinnis@solidice.com
  @date       april 29, 2000
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#ifndef PTINTM_H
#define PTINTM_H

#include <pointintime.h>

namespace go {
  
/*!
  @class PtInTm
   
  <b>Description:</b>

  <b>Notes:</b>
   
  @author     rmcinnis@solidice.com 
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
struct PtInTm
{
  uint32_t      tm ;
  uint16_t     ms ;
  uint16_t     dst_tz ;

             PtInTm() ;
             PtInTm( const PtInTm &t ) ;
             PtInTm( const PointInTime &t ) ;

  void       clear() ;
  void       decodeHost() ;
  void       encodeNetwork() ;
  bool       isNull() ;

  // operator overloads
  PtInTm    &operator= ( const PtInTm &t ) ;
  PtInTm    &operator= ( const PointInTime &t ) ;

  // equivalence operators
  bool       operator==( const PtInTm &t ) const ;
  bool       operator!=( const PtInTm &t ) const ;
  bool       operator<( const PtInTm &t ) const ;
  bool       operator<=( const PtInTm &t ) const ;
  bool       operator>( const PtInTm &t ) const ;
  bool       operator>=( const PtInTm &t ) const ;

  // cast operators
  operator PointInTime() const ;
} ; // struct PtInTm

} // namespace go 

#endif

