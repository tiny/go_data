/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       arglist.cpp
  @brief      ArgList class implementation

  @author     rmcinnis@solidice.com
  @date       june 20, 1999
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#include <string.h>
#include <arglist.h>

namespace go {

void ArgList :: increase() 
{
  uint16_t          maxArgs = _maxArgs + 5 ;
  PBVParameter   *tmp = (PBVParameter*)new byte[ sizeof(PBVParameter) * maxArgs ] ;
  memset( tmp, 0x00, sizeof(PBVParameter) * maxArgs ) ;

  if (_args != nullptr)
  {
    memcpy( tmp, _args, sizeof(PBVParameter) * _nArgs ) ;
  }

  if (_args != _set_args)
  {
    delete _args ;
  }
  _args = tmp ;
  _maxArgs = maxArgs ;
} // ArgList :: increase

ArgList &ArgList :: addArg( const ArgList &args_ ) 
{
  for (uint16_t i = 0; i < args_._nArgs; i++)
  {
    addArg( args_._args[i] ) ;
  }

  return *this ;
} // ArgList :: addArg

ArgList &ArgList :: addArg( PBVParameter *arg )
{
  return (arg == nullptr) ? *this : addArg( *arg ) ;
} // ArgList :: addArg

ArgList &ArgList :: addArg( const PBVParameter &arg )
{
  if (_nArgs >= _maxArgs)
    increase() ;

  _args[ _nArgs++ ] = arg ;
  return *this ;
} // ArgList :: addArg

} // namespace go


