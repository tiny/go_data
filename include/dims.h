/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       dims.h
  @brief      Dimension class definition

  @author     rmcinnis@solidice.com
  @date       may 17, 1994
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#ifndef DIMS_H
#define DIMS_H

#include <enum.h>

namespace go {

/*!
  @class Dimension
   
  <b>Description:</b>
   
  <b>Notes:</b>
  
   
  @author     rmcinnis@solidice.com 
  @date       may 17, 1994
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
class Dimension
{
  private  :
    int32_t           _w ;
    int32_t           _h ;

  public   :  
                   Dimension( int32_t w = 0L, int32_t h = 0L ) ;
                   Dimension( const Dimension &d ) ;
    Dimension     &operator= ( const Dimension &d ) ;
    Dimension      operator+ ( const Dimension &d ) ;
    Dimension      operator- ( const Dimension &d ) ;
    bool           operator< ( const Dimension &d ) ;
    bool           operator<=( const Dimension &d ) ;
    bool           operator> ( const Dimension &d ) ;
    bool           operator>=( const Dimension &d ) ;
    bool           operator==( const Dimension &d ) ;
    bool           operator!=( const Dimension &d ) ;

    // access methods
    int32_t          &width() { return _w ; }
    int32_t           width() const { return _w ; }
    int32_t          &height() { return _h ; }
    int32_t           height() const { return _h ; }
} ; // class Dimension

} // namespace go 

#endif
