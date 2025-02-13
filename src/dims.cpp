/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       dims.cpp
  @brief      Dimension class implementation

  @author     rmcinnis@solidice.com
  @date       may 17, 1994
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#include <dims.h>

namespace go {

Dimension :: Dimension( int32_t w, int32_t h )
{
  _w = w ;
  _h = h ;
} // Dimension [constructor]

Dimension :: Dimension( const Dimension &d )
{
  *this = d ;
} // Dimension [constructor]

Dimension &Dimension :: operator= ( const Dimension &d )
{
  _w = d._w ;
  _h = d._h ;
  return *this ;
} // Dimension :: operator=

Dimension Dimension :: operator+ ( const Dimension &d )
{
  return Dimension( _w+d._w, _h+d._h ) ;
} // Dimension :: operator+

Dimension Dimension :: operator- ( const Dimension &d )
{
  return Dimension( _w-d._w, _h-d._h ) ;
} // Dimension :: operator-

bool Dimension :: operator< ( const Dimension &d )
{
  return ((_w < d._w) && (_h < d._h)) ;
} // Dimension :: operator<

bool Dimension :: operator<=( const Dimension &d )
{
  return ((_w <= d._w) && (_h <= d._h)) ;
} // Dimension :: operator<=

bool Dimension :: operator> ( const Dimension &d )
{
  return ((_w > d._w) && (_h > d._h)) ;
} // Dimension :: operator>

bool Dimension :: operator>=( const Dimension &d )
{
  return ((_w >= d._w) && (_h >= d._h)) ;
} // Dimension :: operator>=

bool Dimension :: operator==( const Dimension &d )
{
  return ((_w == d._w) && (_h == d._h)) ;
} // Dimension :: operator==

bool Dimension :: operator!=( const Dimension &d )
{
  return ((_w != d._w) || (_h != d._h)) ;
} // Dimension :: operator!=

} // namespace go 
