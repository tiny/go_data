/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       pbvparameter.h
  @brief      PBVParameter class definition

  @author     rmcinnis@solidice.com
  @date       june 20, 1999
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#ifndef PBVPARAMETER_H
#define PBVPARAMETER_H

#include "enum.h"

namespace go {

enum CZTYPE
{
	UNKNOWN = 0   , // 0
	CZSTRING      , // 1
	CZSTRINGBUFFER, // 2
	CZINTEGER     , // 3
  CZLONG        , // 4
  CZVOIDPTR     , // 5
	CZFLOAT       , // 6
	CZDOUBLE      , // 7
	CZBOOLEAN     , // 8
	CZSHORT         // 9
};

struct PBVVariant
{
  byte       *data ;
  uint16_t      sz ;
} ;

/*!
  @class PBVParameter
   
  <b>Description:</b>
  pass by value parameter class   

  <b>Notes:</b>
   
  @author     rmcinnis@solidice.com 
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
class PBVParameter
{
  public    :
    int16_t          _type ;
    union PBVType
    {
      PBVVariant   variant ;
      int32_t         lValue ;
      float        fValue ;
      double       dValue ;
      int16_t        sValue ;
      int32_t          iValue ;
      bool         bValue ;
      void        *pValue ;
    } _data ;

    void           initialize( int16_t type_spec, void *d, int16_t sz ) ;

                   PBVParameter() ;
                   PBVParameter( int16_t type_spec, void *data, int16_t sz ) ;
                   // int32_trinsic types
                   PBVParameter( bool value ) ;
                   PBVParameter( int32_t value ) ;
                   PBVParameter( int16_t value ) ;
                   PBVParameter( void * value ) ;
                   PBVParameter( float value ) ;
                   PBVParameter( double value ) ;
                   PBVParameter( const char *value, int16_t sz=-1 ) ; // lpsz
                   PBVParameter( PBVParameter &p ) ;
                  ~PBVParameter() ;
    PBVParameter  &operator= ( const PBVParameter &p ) ;

    void           clear() ;
    void           set( int16_t type_spec, void *data, int16_t sz ) ;
    void           set( bool value ) ;
    void           set( int32_t value ) ;
    void           set( int16_t value ) ;
    void           set( void *value ) ;
    void           set( float value ) ;
    void           set( double value ) ;
    void           set( const char *value, int16_t sz=-1 ) ;

                   operator double() const { return asDouble() ; }
                   operator float() const { return asFloat() ; }
                   operator int32_t() const { return asLong() ; }
                   operator int16_t() const { return asShort() ; }
                   operator const char *() const { return asString() ; }

    // access methods
    int16_t          type() const { return _type ; }
    byte          *data() { return _data.variant.data ; }
    int16_t          sz() const { return _data.variant.sz ; }
    bool           asBool() const { return _data.bValue ; }
    double         asDouble() const { return _data.dValue ; }
    float          asFloat() const { return _data.fValue ; }
    int32_t            asInt() const { return _data.iValue ; }
    int32_t           asLong() const { return _data.lValue ; }
    int16_t          asShort() const { return _data.sValue ; }
    const char    *asString() const { return (const char*)_data.variant.data ; }
    void *         asVoid() const { return _data.pValue ; }
} ; // class PBVParameter

} // namespace go

#endif


