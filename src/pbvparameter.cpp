/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       pbvparameter.cpp
  @brief      PBVParameter class implementation

  @author     rmcinnis@solidice.com
  @date       june 20, 1999
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#include <pbvparameter.h>
#include <stdlib.h>
#include <string.h>

namespace go {

void PBVParameter :: initialize( int16_t type_spec, void *d, int16_t sz )
{
  _type = type_spec ;
  switch (_type)
  {
    case CZINTEGER  :
      _data.iValue = *((int32_t *)d) ;
      break ;
    case CZLONG     :
      _data.lValue = *((int32_t *)d) ;
      break ;
    case CZFLOAT    :
      _data.fValue = *((float *)d) ;
      break ;
    case CZDOUBLE   :
      _data.dValue = *((double *)d) ;
      break ;
    case CZBOOLEAN  :
      _data.bValue = *((bool *)d) ;
      break ;
    case CZSHORT    :
      _data.sValue = *((int16_t *)d) ;
      break ;
    case CZVOIDPTR  :
      _data.pValue = ((void *)d) ;
      break ;
    case UNKNOWN    : // do nothing
      break ; 
    default  :
      if (d == nullptr) 
      {
        _data.variant.data = nullptr ;
        _data.variant.sz   = 0 ;
      }
      else
      {
        _data.variant.data = new byte[(_data.variant.sz = sz)] ;
        memcpy( _data.variant.data, d, _data.variant.sz ) ;
      }
      break ;
  }

} // PBVParameter :: initialize

PBVParameter :: PBVParameter() 
{
  _type = UNKNOWN ;
  _data.lValue = 0 ;
} // PBVParameter [constructor]

PBVParameter :: PBVParameter( int16_t type_spec, void *data, int16_t sz )
{
  initialize( type_spec, data, sz ) ;
} // PBVParameter [constructor]

PBVParameter :: PBVParameter( bool value )
{
  _type = CZBOOLEAN ;
  _data.bValue = value ;
//  initialize( CZBOOLEAN, &value, sizeof(bool) ) ;
} // PBVParameter [constructor]

PBVParameter :: PBVParameter( int32_t value )
{
  _type = CZLONG ;
  _data.lValue = value ;
//  initialize( CZLONG, &value, sizeof(int32_t) ) ;
} // PBVParameter [constructor]

PBVParameter :: PBVParameter( void *value )
{
  initialize( CZVOIDPTR, value, sizeof(void *) ) ;
} // PBVParameter [constructor]

PBVParameter :: PBVParameter( int16_t value )
{
  _type = CZSHORT ;
  _data.sValue = value ;
//  initialize( CZSHORT, &value, sizeof(int16_t) ) ;
} // PBVParameter [constructor]

PBVParameter :: PBVParameter( float value )
{
  _type = CZFLOAT ;
  _data.fValue = value ;
//  initialize( CZFLOAT, &value, sizeof(float) ) ;
} // PBVParameter [constructor]

PBVParameter :: PBVParameter( double value )
{
  _type = CZDOUBLE ;
  _data.dValue = value ;
//  initialize( CZDOUBLE, &value, sizeof(double) ) ;
} // PBVParameter [constructor]

PBVParameter :: PBVParameter( const char *value, int16_t sz )
{
  initialize( CZSTRING, (void *)value, (sz == -1) ? strlen(value)+1 : sz+1 ) ;
} // PBVParameter [constructor]

PBVParameter :: PBVParameter( PBVParameter &p ) 
{
  _type = CZLONG ; // simple type
  _data.variant.data = nullptr ;
  *this = p ;
} // PBVParameter [constructor]

PBVParameter :: ~PBVParameter()
{
  if (_type != UNKNOWN)
    clear() ;
} // PBVParameter [destructor]

PBVParameter &PBVParameter :: operator= ( const PBVParameter &p )
{
  // clean up if necessary
  if (_type != UNKNOWN)
    clear() ;

  // grab new value
  _type = p._type ;
  switch (p._type)
  {
    case CZINTEGER  :
      _data.iValue = p._data.iValue ;
      break ;
    case CZLONG     :
      _data.lValue = p._data.lValue ;
      break ;
    case CZFLOAT    :
      _data.fValue = p._data.fValue ;
      break ;
    case CZDOUBLE   :
      _data.dValue = p._data.dValue ;
      break ;
    case CZBOOLEAN  :
      _data.bValue = p._data.bValue ;
      break ;
    case CZSHORT    :
      _data.sValue = p._data.sValue ;
      break ;
    case CZVOIDPTR  :
      _data.pValue = p._data.pValue ;
      break ;
    case UNKNOWN    :
      break ;
    default         : // all others are variant - grab copy
      initialize( p._type, p._data.variant.data, p._data.variant.sz ) ;
      break ;
  }
  return *this ;
} // PBVParameter :: operator=

void PBVParameter :: clear() 
{
  switch (_type)
  {
    case CZINTEGER  :
    case CZLONG     :
    case CZFLOAT    :
    case CZDOUBLE   : 
    case CZBOOLEAN  :
    case CZSHORT    :
    case CZVOIDPTR  :
    case UNKNOWN    :
      break ;
    default         : // all others are variant
      delete _data.variant.data ;
      _data.variant.data = nullptr ;
      _data.variant.sz   = 0 ;
      break ;
  }
  // just in case
  _type = UNKNOWN ;
} // PBVParameter :: clear

void PBVParameter :: set( int16_t type_spec, void *data, int16_t sz ) 
{
  initialize( type_spec, data, sz ) ;
} // PBVParameter :: set

void PBVParameter :: set( bool value ) 
{
  _type = CZBOOLEAN ;
  _data.bValue = value ;
} // PBVParameter :: set

void PBVParameter :: set( int16_t value ) 
{
  _type = CZSHORT ;
  _data.sValue = value ;
} // PBVParameter :: set

void PBVParameter :: set( int32_t value ) 
{
  _type = CZLONG ;
  _data.lValue = value ;
} // PBVParameter :: set

void PBVParameter :: set( void *value ) 
{
//  initialize( CZVOIDPTR, value, sizeof(void *) ) ;
  _type = CZVOIDPTR ;
  _data.pValue = value ;
} // PBVParameter :: set

void PBVParameter :: set( float value ) 
{
  _type = CZFLOAT ;
  _data.fValue = value ;
} // PBVParameter :: set

void PBVParameter :: set( double value ) 
{
  _type = CZDOUBLE ;
  _data.dValue = value ;
} // PBVParameter :: set

void PBVParameter :: set( const char *value, int16_t sz )
{
  if (value == nullptr)
    initialize( CZSTRING, (void *)value, 0 ) ;
  else
    initialize( CZSTRING, (void *)value, (sz == -1) ? strlen(value)+1 : sz+1 ) ;
} // PBVParameter :: set

} // namespace go 
