/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       arglist.h
  @brief      ArgList class definition

  @author     rmcinnis@solidice.com
  @date       june 20, 1999
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#ifndef ARGLIST_H
#define ARGLIST_H

#include "enum.h"
#include "pbvparameter.h"
#include <vector>

namespace go{
/*!
  @class ArgList
   
  <b>Description:</b>
   
  <b>Notes:</b>
  implemented using variant array instead of STL vector for performance reasons
   
  @author     rmcinnis@solidice.com 
  @date       june-20-1999
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
class ArgList
{
  private   :
    // using union to allocate 4 args to start to avoid an additional new for first 4 args
    PBVParameter                  _set_args[6] ;
    PBVParameter                 *_args ;
    uint16_t                        _nArgs ;
    uint16_t                        _maxArgs ;

  protected :
    void                          increase() ;

  public    :
    inline                        ArgList() { init() ; }
    inline                        ArgList( const ArgList &args ) { init() ; addArg( args ) ; }
    inline                       ~ArgList() { clear() ; }

    inline void                   clear() 
                                  {
                                    if (_args == nullptr)
                                    {
                                      init() ;
                                      return ;
                                    }

                                    for (uint16_t i = 0; i < _nArgs; i++)
                                    {
                                      switch (_args[i]._type)
                                      {
                                        case CZSTRING       :
                                        case CZSTRINGBUFFER :
                                          delete _args[i]._data.variant.data ;
                                          _args[i]._type = UNKNOWN ;
                                          break ;
                                      }
                                    }

                                    if (_args != _set_args)
                                    {
                                      delete _args ;
                                      _args = _set_args ;
                                    }
                                    _nArgs   = 0 ;
                                    _maxArgs = 6 ;
                                  }
    inline PBVParameter          *getArg( uint16_t n = 0 ) const 
                                  { return (n >= size()) ? nullptr : &_args[n] ; }
    inline void                   init() { _args = _set_args ; _nArgs = 0 ; _maxArgs = 6 ; } // becareful with this
    inline uint16_t                 size() const { return _nArgs ; }
    ArgList &                     addArg( const ArgList &args_ );
    ArgList &                     addArg( PBVParameter *arg );
    ArgList &                     addArg( const PBVParameter &arg );

    inline ArgList               &operator<< ( const ArgList &args_ ) { return addArg( args_ ) ; }
    inline ArgList               &operator<< ( PBVParameter *arg ) { return addArg( arg ) ; }
    inline ArgList               &operator<< ( bool arg ) 
                                  {
                                    if (_nArgs >= _maxArgs)
                                      increase() ;

                                    _args[ _nArgs ]._type = CZBOOLEAN ;
                                    _args[ _nArgs++ ]._data.bValue = arg ;
                                    return *this ;
                                  }
    inline ArgList               &operator<< ( int16_t arg ) 
                                  {
                                    if (_nArgs >= _maxArgs)
                                      increase() ;

                                    _args[ _nArgs ]._type = CZSHORT ;
                                    _args[ _nArgs++ ]._data.sValue = arg ;
                                    return *this ;
                                  }
    inline ArgList               &operator<< ( int32_t arg ) 
                                  {
                                    if (_nArgs >= _maxArgs)
                                      increase() ;

                                    _args[ _nArgs ]._type = CZLONG ;
                                    _args[ _nArgs++ ]._data.lValue = arg ;
                                    return *this ;
                                  }
    inline ArgList               &operator<< ( void *arg ) 
                                  {
                                    if (_nArgs >= _maxArgs)
                                      increase() ;

                                    _args[ _nArgs ]._type = CZVOIDPTR ;
                                    _args[ _nArgs++ ]._data.pValue = arg ;
                                    return *this ;
                                  }
    inline ArgList               &operator<< ( float arg ) 
                                  {
                                    if (_nArgs >= _maxArgs)
                                      increase() ;

                                    _args[ _nArgs ]._type = CZFLOAT ;
                                    _args[ _nArgs++ ]._data.fValue = arg ;
                                    return *this ;
                                  }
    inline ArgList               &operator<< ( double arg ) 
                                  {
                                    if (_nArgs >= _maxArgs)
                                      increase() ;

                                    _args[ _nArgs ]._type = CZDOUBLE ;
                                    _args[ _nArgs++ ]._data.dValue = arg ;
                                    return *this ;
                                  }
    inline ArgList               &operator<< ( const char *arg ) 
                                  {
                                    if (_nArgs >= _maxArgs)
                                      increase() ;

                                    _args[ _nArgs++ ].set( arg ) ;
                                    return *this ;
                                  }

    bool                          isBad(int16_t n) const
                                  { return( nullptr == getArg(n) ); }
    int16_t                         sizeOf(int16_t n) const
                                  { PBVParameter *p = getArg(n); return( p?p->sz():0 ); }
    bool                          asBool(int16_t n) const
                                  { PBVParameter *p = getArg(n); return( p?p->asBool():0 ); }
    int32_t                           asInt(int16_t n) const
                                  { PBVParameter *p = getArg(n); return( p?p->asInt():-1 ); }
    int16_t                         asShort(int16_t n) const
                                  { PBVParameter *p = getArg(n); return( p?p->asShort():-1 ); }
    int32_t                          asLong(int16_t n) const
                                  { PBVParameter *p = getArg(n); return( p?p->asLong():-1 ); }
    float                         asFloat(int16_t n) const
                                  { PBVParameter *p = getArg(n); return( p?p->asFloat():-1 ); }
    double                        asDouble(int16_t n) const
                                  { PBVParameter *p = getArg(n); return( p?p->asDouble():-1 ); }
    void                         *asVoid(int16_t n) const
                                  { PBVParameter *p = getArg(n); return( p?p->asVoid():nullptr ); }

} ; // class ArgList

} // namespace go

#endif


