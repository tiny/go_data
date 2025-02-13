/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       goobject.cpp
  @brief      GOObject class implementation

  @author     rmcinnis@solidice.com
  @date       january 12, 1998
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#include <goobject.h>

namespace go {

#pragma warning( disable : 4355 )
GOObject :: GOObject()
: _valueCB( this )
{
} // GOObject [constructor]

GOObject :: GOObject( const GOObject &i )
: _valueCB( this )
{
  _valueCB = i._valueCB ;
} // GOObject [constructor]

GOObject :: ~GOObject()
{
} // GOObject [destructor]

} // namespace go  

